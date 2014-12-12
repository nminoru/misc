#include "postgres.h"

#include "fmgr.h"
#include "utils/elog.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(throw_test);

Datum
throw_test(PG_FUNCTION_ARGS)
{
	PG_TRY();
	{
		PG_TRY();
		{
			ereport(ERROR,
					(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
					 errmsg("throw_test must be loaded via shared_preload_libraries")));
		}
		PG_CATCH();
		{
			printf("Caught(1)\n");
			PG_RE_THROW();
		}
		PG_END_TRY();
	}
	PG_CATCH();
	{
		printf("Caught(2)\n");
		/* PG_RE_THROW(); */
	}
	PG_END_TRY();

	PG_RETURN_VOID();
}

