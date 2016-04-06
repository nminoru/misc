#include "postgres.h"

#include "access/heapam.h"
#include "fmgr.h"
#include "port.h"
#include "storage/lock.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/relcache.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(lock_test2);
Datum
lock_test2(PG_FUNCTION_ARGS)
{
	Oid			table_oid;
	text	   *lock_type;
	float8		sleep_time;
	const char *lock_type_str;
	LOCKMODE	lockmode;
	Relation	heapRelation;

	table_oid	= PG_GETARG_OID(0);
	lock_type	= PG_GETARG_TEXT_P(1);
	sleep_time	= PG_GETARG_FLOAT8(2);

	lock_type_str = text_to_cstring(lock_type);

	if (pg_strcasecmp(lock_type_str, "NOLOCK") == 0)
		lockmode = NoLock;
	else if (pg_strcasecmp(lock_type_str, "ACCESSSHARELOCK") == 0)
		lockmode = AccessShareLock;
	else if (pg_strcasecmp(lock_type_str, "ROWSHARELOCK") == 0)
		lockmode = RowShareLock;
	else if (pg_strcasecmp(lock_type_str, "ROWEXCLUSIVELOCK") == 0)
		lockmode = RowExclusiveLock;
	else if (pg_strcasecmp(lock_type_str, "SHAREUPDATEEXCLUSIVELOCK") == 0)
		lockmode = ShareUpdateExclusiveLock;
	else if (pg_strcasecmp(lock_type_str, "SHARELOCK") == 0)
		lockmode = ShareLock;
	else if (pg_strcasecmp(lock_type_str, "SHAREEXCLUSIVELOCK") == 0)
		lockmode = ShareRowExclusiveLock;
	else if (pg_strcasecmp(lock_type_str, "EXCLUSIVELOCK") == 0)
		lockmode = ExclusiveLock;
	else if (pg_strcasecmp(lock_type_str, "ACCESSEXCLUSIVELOCK") == 0)
		lockmode = AccessExclusiveLock;
	else
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("\"lock_mode\" is set to invalid string: %s", lock_type_str),
				 errhint("\"lock_mode\" must select one among NoLock, AccessShareLock, RowShareLock, RowExclusiveLock, ShareUpdateExclusiveLock, ShareLock, ShareExclusiveLock, ExclusiveLock, and AccessExclusiveLock")));

	elog(NOTICE, "enter lock %d as %s", table_oid, lock_type_str);

	heapRelation = relation_open(table_oid, lockmode);

	elog(NOTICE, "succeed locking %d as %s", table_oid, lock_type_str);

	pg_usleep(sleep_time * 1000000L);

	relation_close(heapRelation, lockmode);

	elog(NOTICE, "exit lock %d as %s", table_oid, lock_type_str);

	PG_RETURN_VOID();
}
