#include "postgres.h"

#include "fmgr.h"
#include "miscadmin.h"
#include "storage/ipc.h"
#include "storage/shmem.h"

typedef struct
{
	int32  counter;

} shared_memory_env;

PG_MODULE_MAGIC;

extern void _PG_init(void);

static shmem_startup_hook_type shmem_startup_prev = NULL;
static shared_memory_env * volatile shared_memory;

static void shmem_test_setup(void);

void
_PG_init(void)
{
    if (!process_shared_preload_libraries_in_progress)
        return;

    RequestAddinShmemSpace(sizeof(shared_memory_env));

    shmem_startup_prev = shmem_startup_hook;
    shmem_startup_hook = shmem_test_setup;
}


static void
shmem_test_setup(void)
{
    void *address;
    bool found;

	if (shmem_startup_prev)
		shmem_startup_prev();

    address = ShmemInitStruct("shared test memory", sizeof(shared_memory_env), &found);

    Assert(!found);

	shared_memory = (shared_memory_env *) address;
}


PG_FUNCTION_INFO_V1(show_shm_test);
Datum
show_shm_test(PG_FUNCTION_ARGS)
{
	int32 result;

	if (!shared_memory)
	{
		void *address;
		bool found;

		address = ShmemInitStruct("shared test memory", sizeof(shared_memory_env), &found);
		shared_memory = (shared_memory_env *) address;
	}

	result = ++shared_memory->counter;

	PG_RETURN_INT32(result);
}
