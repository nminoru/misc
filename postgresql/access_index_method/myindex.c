#include "postgres.h"

#include <limits.h>

#include "fmgr.h"


PG_MODULE_MAGIC;


PG_FUNCTION_INFO_V1(myindex_build);
Datum
myindex_build(PG_FUNCTION_ARGS)
{
	Relation	heapRel = (Relation) PG_GETARG_POINTER(0);
	Relation	indexRel = (Relation) PG_GETARG_POINTER(1);
	IndexInfo  *indexInfo = (IndexInfo *) PG_GETARG_POINTER(2);
	IndexBuildResult *result;

	result = NULL;

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(myindex_buildempty);
Datum
myindex_buildempty(PG_FUNCTION_ARGS)
{
	Relation indexRel = (Relation) PG_GETARG_POINTER(0);

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(myindex_insert);
Datum
myindex_insert(PG_FUNCTION_ARGS)
{
	Relation    indexRel= (Relation) PG_GETARG_POINTER(0);
	Datum      *values  = (Datum *) PG_GETARG_POINTER(1);
	bool       *isnull  = (bool *) PG_GETARG_POINTER(2);
	ItemPointer heap_tid = (ItemPointer) PG_GETARG_POINTER(3);
	Relation    heapRel = (Relation) PG_GETARG_POINTER(4);
	IndexUniqueCheck checkUnique = (IndexUniqueCheck) PG_GETARG_INT32(5);
	bool        result;

	result = false;

	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(myindex_bulkdelete);
Datum
myindex_bulkdelete(PG_FUNCTION_ARGS)
{
	IndexVacuumInfo *info = (IndexVacuumInfo *) PG_GETARG_POINTER(0);
	IndexBulkDeleteResult *volatile stats = (IndexBulkDeleteResult *) PG_GETARG_POINTER(1);
	IndexBulkDeleteCallback callback = (IndexBulkDeleteCallback) PG_GETARG_POINTER(2);
	void	   *callback_state = (void *) PG_GETARG_POINTER(3);

	stats = NULL;

	PG_RETURN_POINTER(stats);
}

PG_FUNCTION_INFO_V1(myindex_vacuumcleanup);
Datum
myindex_vacuumcleanup(PG_FUNCTION_ARGS)
{
	IndexVacuumInfo *info = (IndexVacuumInfo *) PG_GETARG_POINTER(0);
	IndexBulkDeleteResult *stats = (IndexBulkDeleteResult *) PG_GETARG_POINTER(1);

	PG_RETURN_POINTER(stats);
}

PG_FUNCTION_INFO_V1(myindex_canreturn);
Datum
myindex_canreturn(PG_FUNCTION_ARGS)
{
	Relation indexRel= (Relation) PG_GETARG_POINTER(0);
	bool result;

	/* pass */
	result = false;

	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(myindex_costestimate);
Datum
myindex_costestimate(PG_FUNCTION_ARGS)
{
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	IndexPath  *path = (IndexPath *) PG_GETARG_POINTER(1);
	double		loop_count = PG_GETARG_FLOAT8(2);
	Cost	   *indexStartupCost = (Cost *) PG_GETARG_POINTER(3);
	Cost	   *indexTotalCost = (Cost *) PG_GETARG_POINTER(4);
	Selectivity *indexSelectivity = (Selectivity *) PG_GETARG_POINTER(5);
	double	   *indexCorrelation = (double *) PG_GETARG_POINTER(6);

	/* always return worst cost value */
	*indexStartupCost = DBL_MAX;
	*indexTotalCost   = DBL_MAX;
	*indexSelectivity = 1.0;
	*indexCorrelation = 0.0;

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(myindex_options);
Datum
myindex_options(PG_FUNCTION_ARGS)
{
	Datum		reloptions = PG_GETARG_DATUM(0);
	bool		validate = PG_GETARG_BOOL(1);
	bytea	   *result;

	/* pass */
	PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(myindex_beginscan);
Datum
myindex_beginscan(PG_FUNCTION_ARGS)
{
	Relation indexRel = (Relation) PG_GETARG_POINTER(0);
	int		 nkeys = PG_GETARG_INT32(1);
	int		 norderbys = PG_GETARG_INT32(2);
	IndexScanDesc result;

	/* pass */
	result = NULL;

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(myindex_rescan);
Datum
myindex_rescan(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);
	ScanKey keys = (ScanKey) PG_GETARG_POINTER(1);
	int		 nkeys = PG_GETARG_INT32(2);
	ScanKey orderbys = (ScanKey) PG_GETARG_POINTER(3);
	int		norderbys = PG_GETARG_INT32(4);

	/* pass */
	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(myindex_gettuple);
Datum
myindex_gettuple(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);
	ScanDirection direction = (ScanDirection) PG_GETARG_INT32(1);
	bool		result;

	/* pass */
	result = false;

	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(myindex_getbitmap);
Datum
myindex_getbitmap(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);
	TIDBitmap  *tbm = (TIDBitmap *) PG_GETARG_POINTER(1);
	int64		result;

	/* pass */
	result = 0;

	PG_RETURN_INT64(result);
}

PG_FUNCTION_INFO_V1(myindex_endscan);
Datum
myindex_endscan(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);

	/* pass */
	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(myindex_markpos);
Datum
myindex_markpos(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);

	/* pass */
	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(myindex_restrpos);
Datum
myindex_restrpos(PG_FUNCTION_ARGS)
{
	IndexScanDesc scan = (IndexScanDesc) PG_GETARG_POINTER(0);

	/* pass */
	PG_RETURN_VOID();
}
