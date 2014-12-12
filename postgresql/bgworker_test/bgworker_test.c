#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


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


#if PG_VERSION_NUM < 90300
#error "This program can work 9.3 or older version."
#endif


PG_MODULE_MAGIC;

extern void _PG_init(void);
extern void worker_spi_main(Datum);

static const Size shmem_size = 1024UL;
static shmem_startup_hook_type shmem_startup_prev = NULL;

static void shmem_test_setup(void);

void
_PG_init(void)
{
	BackgroundWorker worker;

	if (!process_shared_preload_libraries_in_progress)
		ereport(ERROR,
				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
				 errmsg("bgworker_test must be loaded via shared_preload_libraries")));

    RequestAddinShmemSpace(shmem_size);

    shmem_startup_prev      = shmem_startup_hook;
    shmem_startup_hook      = shmem_test_setup;

	worker.bgw_flags        = BGWORKER_SHMEM_ACCESS |
		BGWORKER_BACKEND_DATABASE_CONNECTION;
	worker.bgw_start_time   = BgWorkerStart_RecoveryFinished;
	worker.bgw_restart_time = BGW_NEVER_RESTART;
	worker.bgw_main         = worker_spi_main;
	worker.bgw_notify_pid   = 0;
	worker.bgw_main_arg     = Int32GetDatum(0);

	RegisterBackgroundWorker(&worker);
}

static void
shmem_test_setup(void)
{
    void *address;
    bool found;

	if (shmem_startup_prev)
		shmem_startup_prev();

	printf("shmem_test_setup\n");

#if 0
    address = ShmemInitStruct("shared memory", shmem_size, &found);
    Assert(!address);
	
	ShmemAddr = address;
#endif
}

void
worker_spi_main(Datum main_arg)
{
	printf("worker_spi_main\n");

	proc_exit(1);
}
