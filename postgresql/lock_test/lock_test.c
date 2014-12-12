#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "access/genam.h"
#include "access/heapam.h"
#include "catalog/namespace.h"
#include "catalog/pg_index.h"
#include "catalog/pg_type.h"
#include "miscadmin.h"
#include "storage/ipc.h"
#include "storage/lwlock.h"
#include "storage/spin.h"
#include "utils/builtins.h"
#include "utils/rel.h"
#include "utils/relcache.h"

PG_MODULE_MAGIC;

typedef struct lock_test_state
{
	LWLock			lock;
	volatile int	count;
	pid_t			last_owner;
} lock_test_state;

extern void _PG_init(void);
static void shmem_test_setup(void);

static shmem_startup_hook_type shmem_startup_prev = NULL;
static lock_test_state *ShmemAddr;

void
_PG_init(void)
{
    if (!process_shared_preload_libraries_in_progress)
		ereport(ERROR,
				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
				 errmsg("lock_test must be loaded via shared_preload_libraries")));

    RequestAddinShmemSpace(sizeof(lock_test_state));

    shmem_startup_prev = shmem_startup_hook;
    shmem_startup_hook = shmem_test_setup;

#if 0
	ereport(INFO,
			(errmsg("lock_test loaded")));
#else
	printf("*** lock_test loaded ***\n");
#endif
}

static void shmem_test_setup(void)
{
    void *address;
    bool found;

	if (shmem_startup_prev)
		shmem_startup_prev();

    address = ShmemInitStruct("lock_test: shared memory", sizeof(lock_test_state), &found);
    Assert(!address);

	ShmemAddr = address;

	LWLockInitialize(&ShmemAddr->lock, 0);

	printf("*** lock_test: shmem_test_setup ***\n");
}

PG_FUNCTION_INFO_V1(lock_test);
Datum
lock_test(PG_FUNCTION_ARGS)
{
	text *tablename = PG_GETARG_TEXT_P(0);
	RangeVar *tableRV;
	Oid tableOid;
	Relation heapRelation;
	List *indexOidList;
	ListCell *l;

	tableRV  = makeRangeVarFromNameList(textToQualifiedNameList(tablename));
	tableOid = RangeVarGetRelid(tableRV, NoLock, false);

	heapRelation = relation_open(tableOid, AccessExclusiveLock);

	indexOidList = RelationGetIndexList(heapRelation);

	foreach(l, indexOidList)
	{
		Oid indexOid;
		Relation indexRelation;
		Form_pg_index index;

		indexOid = lfirst_oid(l);

		indexRelation = index_open(indexOid, AccessShareLock);
		index = indexRelation->rd_index;

		if (IndexIsValid(index))
		{
		}

		index_close(indexRelation, AccessShareLock);
	}

	sleep(20);

	relation_close(heapRelation, AccessExclusiveLock);

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(lwlock_test);
Datum
lwlock_test(PG_FUNCTION_ARGS)
{
	pid_t pid;

	pid = getpid();

	if (!ShmemAddr)
	{
		ereport(ERROR, (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
						errmsg("AAA")));
		goto done;
	}

	LWLockAcquire(&ShmemAddr->lock, LW_EXCLUSIVE);
	printf("lock_test(%d) LWLockAcquire count=%d, last_owner=%d\n", pid, ShmemAddr->count, ShmemAddr->last_owner);
	ShmemAddr->count++;
	sleep(20);
	ShmemAddr->count--;
	ShmemAddr->last_owner = pid;
	printf("lock_test(%d) LWLockRelease count=%d\n", pid, ShmemAddr->count);
	LWLockRelease(&ShmemAddr->lock);

done:
	PG_RETURN_VOID();
}

