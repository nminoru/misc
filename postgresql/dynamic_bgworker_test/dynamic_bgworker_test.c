#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "commands/dbcommands.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "postmaster/bgworker.h"
#include "storage/dsm.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/proc.h"
#include "storage/procarray.h"
#include "storage/procsignal.h"
#include "storage/spin.h"
#include "utils/memutils.h"
#include "utils/resowner.h"

#if PG_VERSION_NUM < 90400
#error "This program can work 9.4 or older version."
#endif

typedef struct
{
	slock_t	mutex;

	int		workers_total;
	int		workers_attached;
	int		workers_ready;

	char    dbname[NAMEDATALEN];

} test_shm_mq_header;

typedef struct
{
	int		nworkers;
	BackgroundWorkerHandle *handle[FLEXIBLE_ARRAY_MEMBER];
} worker_state;


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(worker_test);

extern void worker_test_main(Datum main_arg);

static void wait_for_workers_to_become_ready(worker_state *wstate, volatile test_shm_mq_header *hdr);
static bool check_worker_status(worker_state *wstate);
static bool check_all_workers_terminated(worker_state *wstate);
static void cleanup_background_workers(dsm_segment *seg, Datum arg);
static void handle_sigterm(SIGNAL_ARGS);
static void handle_sighup(SIGNAL_ARGS);


Datum
worker_test(PG_FUNCTION_ARGS)
{
	int i, nworkers;
	dsm_segment *seg;
	test_shm_mq_header *hdr;
	MemoryContext oldcontext;
	worker_state *wstate;

	nworkers = PG_GETARG_INT32(0);

	seg = dsm_create(sizeof(test_shm_mq_header));
	hdr = dsm_segment_address(seg);

	printf("begin worker_test: %d, %p\n", dsm_segment_handle(seg), hdr);

	MemSet(hdr, 0, sizeof(test_shm_mq_header));

	SpinLockInit(&hdr->mutex);

	strncpy(hdr->dbname, get_database_name(MyDatabaseId), sizeof(hdr->dbname));

	oldcontext = MemoryContextSwitchTo(CurTransactionContext);

	wstate = MemoryContextAlloc(TopTransactionContext,
								offsetof(worker_state, handle) +
								sizeof(BackgroundWorkerHandle *) * nworkers);

	MemSet(wstate, 0, offsetof(worker_state, handle) + sizeof(BackgroundWorkerHandle *) * nworkers);

	on_dsm_detach(seg, cleanup_background_workers,
				  PointerGetDatum(wstate));

	for (i = 0; i < nworkers; i++)
	{
		BackgroundWorker worker;

		MemSet(&worker, 0, sizeof(worker));

		worker.bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;
		worker.bgw_start_time = BgWorkerStart_ConsistentState;
		worker.bgw_restart_time = BGW_NEVER_RESTART;
		worker.bgw_main = NULL;		/* new worker might not have library loaded */

		sprintf(worker.bgw_library_name, "worker_test");
		sprintf(worker.bgw_function_name, "worker_test_main");
		snprintf(worker.bgw_name, BGW_MAXLEN, "worker_test %d", i);

		worker.bgw_main_arg = UInt32GetDatum(dsm_segment_handle(seg));
		worker.bgw_notify_pid = MyProcPid;

		if (!RegisterDynamicBackgroundWorker(&worker, &wstate->handle[i]))
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
					 errmsg("could not register background process"),
				 errhint("You may need to increase max_worker_processes.")));

		++wstate->nworkers;
	}

	for (i = 0; i < nworkers; i++)
	{
		BgwHandleStatus status;
		pid_t pid;

		status = WaitForBackgroundWorkerStartup(wstate->handle[i], &pid);

		if (status == BGWH_STOPPED)
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
					 errmsg("could not start background process"),
					 errhint("More details may be available in the server log.")));
		if (status == BGWH_POSTMASTER_DIED)
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
					 errmsg("cannot start background processes without postmaster"),
					 errhint("Kill all remaining database processes and restart the database.")));
		Assert(status == BGWH_STARTED);
	}

	wait_for_workers_to_become_ready(wstate, hdr);

	cancel_on_dsm_detach(seg, cleanup_background_workers,
						 PointerGetDatum(wstate));

	dsm_detach(seg);

	pfree(wstate);

	MemoryContextSwitchTo(oldcontext);


	PG_RETURN_VOID();
}

static void
wait_for_workers_to_become_ready(worker_state *wstate,
								 volatile test_shm_mq_header *hdr)
{
	bool save_set_latch_on_sigusr1;
	bool result = false;

	save_set_latch_on_sigusr1 = set_latch_on_sigusr1;
	set_latch_on_sigusr1 = true;

	PG_TRY();
	{
		for (;;)
		{
			int workers_ready;

			/* If all the workers are ready, we have succeeded. */
			SpinLockAcquire(&hdr->mutex);
			workers_ready = hdr->workers_ready;
			SpinLockRelease(&hdr->mutex);
			if (workers_ready >= wstate->nworkers)
			{
				result = true;
				break;
			}

			/* If any workers (or the postmaster) have died, we have failed. */
			if (!check_worker_status(wstate))
			{
				printf("%s:%u\n", __FUNCTION__, __LINE__);
				result = false;
				break;
			}

			/* Wait to be signalled. */
			WaitLatch(&MyProc->procLatch, WL_LATCH_SET, 0);

			/* An interrupt may have occurred while we were waiting. */
			CHECK_FOR_INTERRUPTS();

			/* Reset the latch so we don't spin. */
			ResetLatch(&MyProc->procLatch);
		}
	}
	PG_CATCH();
	{
		set_latch_on_sigusr1 = save_set_latch_on_sigusr1;
		PG_RE_THROW();
	}
	PG_END_TRY();

	if (!result)
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
				 errmsg("one or more background workers failed to start")));

	/* 終了待ち */
	PG_TRY();
	{
		for (;;)
		{
			/* If any workers (or the postmaster) have died, we have failed. */
			if (check_all_workers_terminated(wstate))
			{
				break;
			}

			/* Wait to be signalled. */
			WaitLatch(&MyProc->procLatch, WL_LATCH_SET, 0);

			/* An interrupt may have occurred while we were waiting. */
			CHECK_FOR_INTERRUPTS();

			/* Reset the latch so we don't spin. */
			ResetLatch(&MyProc->procLatch);
		}
	}
	PG_CATCH();
	{
		set_latch_on_sigusr1 = save_set_latch_on_sigusr1;
		PG_RE_THROW();
	}
	PG_END_TRY();
}

static bool
check_worker_status(worker_state *wstate)
{
	int	i;

	/* If any workers (or the postmaster) have died, we have failed. */
	for (i = 0; i < wstate->nworkers; i++)
	{
		BgwHandleStatus status;
		pid_t	pid;
		
		status = GetBackgroundWorkerPid(wstate->handle[i], &pid);
		if (status == BGWH_STOPPED || status == BGWH_POSTMASTER_DIED)
		{
			printf("check_worker_status: %u %u\n", i, status);
			return false;
		}
	}

	/* Otherwise, things still look OK. */
	return true;
}

static bool
check_all_workers_terminated(worker_state *wstate)
{
	int	i;

	/* If any workers (or the postmaster) have died, we have failed. */
	for (i = 0; i < wstate->nworkers; i++)
	{
		BgwHandleStatus status;
		pid_t	pid;
		
		status = GetBackgroundWorkerPid(wstate->handle[i], &pid);
		if (status != BGWH_STOPPED && status != BGWH_POSTMASTER_DIED)
		{
			return false;
		}
	}

	/* Otherwise, things still look OK. */
	return true;
}

static void
cleanup_background_workers(dsm_segment *seg, Datum arg)
{
	worker_state *wstate = (worker_state *)DatumGetPointer(arg);

	while (wstate->nworkers > 0)
	{
		--wstate->nworkers;
		TerminateBackgroundWorker(wstate->handle[wstate->nworkers]);
	}
}

void
worker_test_main(Datum main_arg)
{
	dsm_segment *seg;
	volatile test_shm_mq_header *hdr;
	PGPROC *registrant;

	pqsignal(SIGHUP,  handle_sighup);
	pqsignal(SIGTERM, handle_sigterm);
	ImmediateInterruptOK = false;
	BackgroundWorkerUnblockSignals();

	printf("worker_test_main: %d\n", DatumGetInt32(main_arg));

	CurrentResourceOwner = ResourceOwnerCreate(NULL, "worker test");

	seg = dsm_attach(DatumGetInt32(main_arg));

	if (seg == NULL)
		ereport(ERROR,
				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
				 errmsg("unable to map dynamic shared memory segment")));

	hdr = dsm_segment_address(seg);

	/* 開始 */
	SpinLockAcquire(&hdr->mutex);
	hdr->workers_ready++;
	hdr->workers_attached++;
	SpinLockRelease(&hdr->mutex);

	registrant = BackendPidGetProc(MyBgworkerEntry->bgw_notify_pid);
	if (registrant == NULL)
	{
		elog(DEBUG1, "registrant backend has exited prematurely");
		proc_exit(1);
	}
	SetLatch(&registrant->procLatch);

	/* Do the work */

	BackgroundWorkerInitializeConnection(hdr->dbname, NULL);

	printf("DSM: %p\n", dsm_segment_address);

#if 0
	SetCurrentStatementStartTimestamp();
	StartTransactionCommand();
	SPI_connect();
	PushActiveSnapshot(GetTransactionSnapshot());
	pgstat_report_activity(STATE_RUNNING, "initializing spi_worker schema");	

	SPI_finish();
	PopActiveSnapshot();
	CommitTransactionCommand();
	pgstat_report_activity(STATE_IDLE, NULL);
#endif

	dsm_detach(seg);

	proc_exit(0);
}

static void
handle_sigterm(SIGNAL_ARGS)
{
	int save_errno = errno;

	/* SIGTERM フラグセット */

	if (MyProc)
		SetLatch(&MyProc->procLatch);

	if (!proc_exit_inprogress)
	{
		InterruptPending = true;
		ProcDiePending = true;
	}

	errno = save_errno;
}

static void
handle_sighup(SIGNAL_ARGS)
{
	int save_errno = errno;

	/* SIGHUP フラグセット */

	if (MyProc)
		SetLatch(&MyProc->procLatch);

	if (!proc_exit_inprogress)
	{
		InterruptPending = true;
		ProcDiePending = true;
	}

	errno = save_errno;
}
