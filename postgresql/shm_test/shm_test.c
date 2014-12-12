#include <stdlib.h>
#include <unistd.h>

#include "postgres.h"
#include "access/relscan.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_type.h"
#include "storage/ipc.h"
#include "miscadmin.h"

static const Size shmem_size = 64UL * 1024UL * 1024UL * 1024UL;

PG_MODULE_MAGIC;

extern void _PG_init(void);

static shmem_startup_hook_type shmem_startup_prev = NULL;

static void *ShmemAddr;

static void shmem_test_setup(void)
{
    void *address;
    bool found;

	if (shmem_startup_prev)
		shmem_startup_prev();

    address = ShmemInitStruct("shared memory", shmem_size, &found);
    Assert(!address);

	printf("shmem_test: pid=%u, shmem=%p\n", getpid(), address);

	ShmemAddr = address;
}

void
_PG_init(void)
{
    if (!process_shared_preload_libraries_in_progress)
        return;

    RequestAddinShmemSpace(shmem_size);

    shmem_startup_prev = shmem_startup_hook;
    shmem_startup_hook = shmem_test_setup;
}
