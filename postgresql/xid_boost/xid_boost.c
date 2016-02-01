#include "postgres.h"

#include "access/xact.h"
#include "fmgr.h"
#include "libpq/pqsignal.h"
#include "miscadmin.h"
#include "postmaster/bgworker.h"
#include "storage/procsignal.h"
#include "utils/snapmgr.h"


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(xid_boost);

Datum
xid_boost(PG_FUNCTION_ARGS)
{
	BackgroundWorker worker;
	BackgroundWorkerHandle *handle;

	worker.bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION | BGWORKER_NO_OUTPUT_LOG;
	worker.bgw_start_time = BgWorkerStart_ConsistentState;
	worker.bgw_restart_time = BGW_NEVER_RESTART;
	worker.bgw_main = NULL;

	sprintf(worker.bgw_library_name, "xid_boost");
	sprintf(worker.bgw_function_name, "xid_boost_main");
	snprintf(worker.bgw_name, BGW_MAXLEN, "xid boost");

	worker.bgw_main_arg = (Datum) 0;
	worker.bgw_notify_pid = MyProcPid;	/* notify by SIG_USR1 */

	if (!RegisterDynamicBackgroundWorker(&worker, &handle))
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_RESOURCES),
				 errmsg("could not register background process"),
				 errhint("You may need to increase max_worker_processes.")));

	PG_RETURN_VOID();
}

static volatile bool stop;

static void
sigterm_handler(SIGNAL_ARGS)
{
    stop = true;
}

void
xid_boost_main(Datum main_arg)
{
    pqsignal(SIGINT,  sigterm_handler);
    pqsignal(SIGQUIT, sigterm_handler);
    pqsignal(SIGTERM, sigterm_handler);

    BackgroundWorkerUnblockSignals();

    BackgroundWorkerInitializeConnection("postgres", NULL);

    while (!stop)
    {
        TransactionId xid;

        CHECK_FOR_INTERRUPTS();

        StartTransactionCommand();
        xid = GetCurrentTransactionId();
        CommitTransactionCommand();
    }
}
