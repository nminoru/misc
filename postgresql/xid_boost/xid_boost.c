/*-------------------------------------------------------------------------
 * xid_boost.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <stdlib.h>
#include <string.h>

#include "access/xact.h"
#include "c.h"
#include "fmgr.h"
#include "libpq/pqsignal.h"
#include "miscadmin.h"
#include "postmaster/bgworker.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/lwlock.h"
#include "storage/proc.h"
#include "storage/procsignal.h"
#include "storage/shmem.h"
#include "utils/guc.h"
#include "utils/guc_tables.h"
#include "utils/snapmgr.h"

#define MAX_WORKERS (64)

extern void _PG_init(void);
extern void xid_boost_daemon_main(Datum main_arg);
extern void xid_boost_worker_main(Datum main_arg);


typedef struct
{
	LWLock	lock;
	
	PGPROC *daemon;

	volatile int num_workers;

	volatile bool shutdown;

	struct
	{
		volatile bool running;
		volatile bool stop;
		BackgroundWorker worker;
		BackgroundWorkerHandle *handle;
	} workers[MAX_WORKERS];

} shmem_data_t;

static shmem_data_t *shmem_data_p;
static shmem_startup_hook_type shmem_startup_prev = NULL;

static void daemon_startup_routine(void);
static void shmem_startup_routine(void);
static void daemon_sigterm_handler(SIGNAL_ARGS);
static void daemon_sigusr1_handler(SIGNAL_ARGS);


PG_MODULE_MAGIC;

void
_PG_init(void)
{
	if (!process_shared_preload_libraries_in_progress)
		ereport(ERROR,
				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
				 errmsg("xid_boost must be loaded via shared_preload_libraries")));

	RequestAddinShmemSpace(sizeof(*shmem_data_p));

	shmem_startup_prev = shmem_startup_hook;
	shmem_startup_hook = shmem_startup_routine;

	daemon_startup_routine();
}

static void
daemon_startup_routine(void)
{
	BackgroundWorker worker;

	memset(&worker, 0, sizeof(worker));

	worker.bgw_flags = BGWORKER_SHMEM_ACCESS;
	worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
	worker.bgw_restart_time = BGW_NEVER_RESTART;
	worker.bgw_notify_pid = 0;
	snprintf(worker.bgw_name, BGW_MAXLEN, "xid_boost:daemon");
	worker.bgw_main = NULL;
	worker.bgw_main_arg = (Datum) 0;
	sprintf(worker.bgw_library_name, "xid_boost");
	sprintf(worker.bgw_function_name, "xid_boost_daemon_main");

	RegisterBackgroundWorker(&worker);
}

static void
shmem_startup_routine(void)
{
	bool found;

	if (shmem_startup_prev)
		shmem_startup_prev();

	shmem_data_p = (shmem_data_t *) ShmemInitStruct("xid_boost: shared memory", sizeof(*shmem_data_p), &found);

	LWLockInitialize(&shmem_data_p->lock, 0);
}

void
xid_boost_daemon_main(Datum main_arg)
{
	shmem_data_p->daemon = MyProc;

	pqsignal(SIGINT,  daemon_sigterm_handler);
	pqsignal(SIGQUIT, daemon_sigterm_handler);
	pqsignal(SIGTERM, daemon_sigterm_handler);
	pqsignal(SIGUSR1, daemon_sigusr1_handler);

	set_latch_on_sigusr1 = true;

	BackgroundWorkerUnblockSignals();

	while (!shmem_data_p->shutdown)
	{
		int i;
		int rc;
		int32 num_workers;

		num_workers = shmem_data_p->num_workers;

		for (i = 0 ; i < MAX_WORKERS ; i++)
		{
			BackgroundWorker *worker;

			worker = &shmem_data_p->workers[i].worker;

			if (i < num_workers)
			{			
				if (!shmem_data_p->workers[i].running)
				{
					shmem_data_p->workers[i].stop = false;

					worker->bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;
					worker->bgw_start_time = BgWorkerStart_ConsistentState;
					worker->bgw_restart_time = BGW_NEVER_RESTART;
					worker->bgw_main = NULL;

					sprintf(worker->bgw_library_name, "xid_boost");
					sprintf(worker->bgw_function_name, "xid_boost_worker_main");
					snprintf(worker->bgw_name, BGW_MAXLEN, "xid_boost:worker");

					worker->bgw_main_arg = (Datum) i;
					worker->bgw_notify_pid = MyProcPid;	/* notify by SIG_USR1 */

					if (!RegisterDynamicBackgroundWorker(worker, &shmem_data_p->workers[i].handle))
						ereport(PANIC,
								(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
								 errmsg("could not register background process"),
								 errhint("You may need to increase max_worker_processes.")));
	
					shmem_data_p->workers[i].running = true;
				}
			}
			else
			{
				if (shmem_data_p->workers[i].running)
				{
					shmem_data_p->workers[i].stop = true;
				}
				else if (shmem_data_p->workers[i].stop)
				{
					BgwHandleStatus status;
					pid_t pid;

					status = GetBackgroundWorkerPid(shmem_data_p->workers[i].handle, &pid);

					if (status == BGWH_STOPPED)
					{
						shmem_data_p->workers[i].running = false;
						shmem_data_p->workers[i].stop = false;
					}
				}
			}
		}

		rc = WaitLatch(&MyProc->procLatch,
					   WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH,
					   10 * 1000L /* 10 second */);
		ResetLatch(&MyProc->procLatch);

		if (rc & WL_POSTMASTER_DEATH)
			proc_exit(1);
	}
}

static void
daemon_sigterm_handler(SIGNAL_ARGS)
{
	shmem_data_p->shutdown = true;

	if (MyProc)
		SetLatch(&MyProc->procLatch);
}

static void
daemon_sigusr1_handler(SIGNAL_ARGS)
{
	procsignal_sigusr1_handler(postgres_signal_arg);
}
void
xid_boost_worker_main(Datum main_arg)
{
	int worker_id;

	worker_id = DatumGetInt32(main_arg);

	BackgroundWorkerUnblockSignals();

	BackgroundWorkerInitializeConnection("postgres", NULL);

	while (!shmem_data_p->shutdown && !shmem_data_p->workers[worker_id].stop)
	{
		TransactionId xid;

		CHECK_FOR_INTERRUPTS();

		StartTransactionCommand();
		xid = GetCurrentTransactionId();
		CommitTransactionCommand();
	}
}

PG_FUNCTION_INFO_V1(xid_boost);

Datum
xid_boost(PG_FUNCTION_ARGS)
{
	int32 num_workers;

	if (PG_NARGS() != 1)
		elog(ERROR, "xid_boost() takes only the number of workers");

	num_workers = PG_GETARG_INT32(0);

	if (num_workers < 0 || MAX_WORKERS < num_workers)
		elog(ERROR, "The argument workers must be from 0 to %d", MAX_WORKERS);

	LWLockAcquire(&shmem_data_p->lock, LW_EXCLUSIVE);
	shmem_data_p->num_workers = num_workers;
	LWLockRelease(&shmem_data_p->lock);

	SetLatch(&shmem_data_p->daemon->procLatch);

	PG_RETURN_INT32(num_workers);
}
