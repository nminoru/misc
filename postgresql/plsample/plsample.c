#include "postgres.h"

#include "access/heapam.h"
#include "access/htup.h"
#include "access/htup_details.h"
#include "access/printtup.h"
#include "access/sdir.h"
#include "access/tupdesc.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "commands/event_trigger.h"
#include "commands/trigger.h"
#include "executor/execdesc.h"
#include "executor/executor.h"
#include "executor/spi.h"
#include "executor/tstoreReceiver.h"
#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "tcop/dest.h"
#include "tcop/pquery.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/snapmgr.h"
#include "utils/syscache.h"
#include "utils/tuplestore.h"



#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


typedef struct
{
	char   *sql_string;
} PLsample_function;


static PlannedStmt *generate_plannedstmt(const char *sql_string);


PG_FUNCTION_INFO_V1(plsample_call_handler);

Datum
plsample_call_handler(PG_FUNCTION_ARGS)
{
	PLsample_function *function;
	FmgrInfo	   *flinfo = fcinfo->flinfo;
	ReturnSetInfo  *rsi;
	TupleDesc		tupdesc;
	MemoryContext	oldcontext;
	DestReceiver   *dest;
	PlannedStmt	   *plannedstmt;
	QueryDesc	   *query_desc;
	int				rc;

	if ((rc = SPI_connect()) != SPI_OK_CONNECT)
		elog(ERROR, "SPI_connect failed: %s", SPI_result_code_string(rc));

	if (CALLED_AS_TRIGGER(fcinfo) || CALLED_AS_EVENT_TRIGGER(fcinfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("plsample's procedural function cannot be called as a trigger function")));

	function = (PLsample_function *) flinfo->fn_extra;

	if (function == NULL)
	{
		Oid			funcOid;
		HeapTuple	procTup;
		Datum		proSrc;
		bool		isnull;

		funcOid = flinfo->fn_oid;

		procTup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
		if (!HeapTupleIsValid(procTup))
			elog(ERROR, "cache lookup failed for cuntion %u", funcOid);

		proSrc = SysCacheGetAttr(PROCOID, procTup, Anum_pg_proc_prosrc, &isnull);
		if (isnull)
			elog(ERROR, "null prosrc");

		oldcontext = MemoryContextSwitchTo(flinfo->fn_mcxt);

		flinfo->fn_extra = palloc0(sizeof(PLsample_function));
		function = (PLsample_function *) flinfo->fn_extra;
		function->sql_string = TextDatumGetCString(proSrc);

		MemoryContextSwitchTo(oldcontext);

		ReleaseSysCache(procTup);
	}

	rsi = (ReturnSetInfo *) fcinfo->resultinfo;

	if (!rsi || !IsA(rsi, ReturnSetInfo) ||
		(rsi->allowedModes & SFRM_Materialize) == 0 ||
		rsi->expectedDesc == NULL)
	{
		printf("rsi: %p\n", rsi);

		if (rsi)
		{
			if (IsA(rsi, ReturnSetInfo))
				printf("allowedModes = %x, expectedDesc: %p\n", rsi->allowedModes, rsi->expectedDesc);
			else
				printf("***\n");
		}

		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	}

	rsi->returnMode = SFRM_Materialize;

	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("function returning record called in context "
						"that cannot accept type record")));

	oldcontext = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);

	rsi->setDesc = CreateTupleDescCopy(tupdesc);
	BlessTupleDesc(rsi->setDesc);
	rsi->setResult = tuplestore_begin_heap(rsi->allowedModes & SFRM_Materialize_Random,
										   false, work_mem);

	MemoryContextSwitchTo(oldcontext);

	dest = CreateDestReceiver(DestTuplestore);
	SetTuplestoreDestReceiverParams(dest, rsi->setResult, CurrentMemoryContext, false);

	plannedstmt = generate_plannedstmt(function->sql_string);

	query_desc = CreateQueryDesc(plannedstmt,
								 function->sql_string,
								 GetActiveSnapshot(), InvalidSnapshot,
								 dest, NULL, 0);

	ExecutorStart(query_desc, 0);

	ExecutorRun(query_desc, ForwardScanDirection, 0L);

	ExecutorFinish(query_desc);
	ExecutorEnd(query_desc);

	(*dest->rDestroy)(dest);

	FreeQueryDesc(query_desc);

	if ((rc = SPI_finish()) != SPI_OK_FINISH)
		elog(ERROR, "SPI_finish failed: %s", SPI_result_code_string(rc));

	PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(plsample_validator);

Datum
plsample_validator(PG_FUNCTION_ARGS)
{
	Oid				funcOid;
	HeapTuple		procTup;
	Datum			proSrc;
	bool			isnull;
	const char	   *pl_src;

	funcOid = PG_GETARG_OID(0);

	if (!CheckFunctionValidatorAccess(fcinfo->flinfo->fn_oid, funcOid))
		PG_RETURN_VOID();

	procTup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
	if (!HeapTupleIsValid(procTup))
		elog(ERROR, "cache lookup failed for cuntion %u", funcOid);

	proSrc = SysCacheGetAttr(PROCOID, procTup, Anum_pg_proc_prosrc, &isnull);
	if (isnull)
		elog(ERROR, "null prosrc");

	pl_src = TextDatumGetCString(proSrc);

	/* @todo insert checking code */

	ReleaseSysCache(procTup);

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(plsample_inline_handler);

Datum
plsample_inline_handler(PG_FUNCTION_ARGS)
{
	InlineCodeBlock *codeblock = (InlineCodeBlock *) PG_GETARG_POINTER(0);
	PlannedStmt	   *plannedstmt;
	DestReceiver   *dest;
	QueryDesc	   *query_desc;

	plannedstmt = generate_plannedstmt(codeblock->source_text);

	dest = CreateDestReceiver(DestRemote);
	SetRemoteDestReceiverParams(dest, ActivePortal);

	query_desc = CreateQueryDesc(plannedstmt, codeblock->source_text,
								 GetActiveSnapshot(), InvalidSnapshot,
								 dest, NULL, 0);

	ExecutorStart(query_desc, 0);

	ExecutorRun(query_desc, ForwardScanDirection, 0L);

	ExecutorFinish(query_desc);
	ExecutorEnd(query_desc);

	FreeQueryDesc(query_desc);

	PG_RETURN_VOID();
}

static PlannedStmt *
generate_plannedstmt(const char *sql_string)
{
	List   *raw_parsetree_list;
	Node   *parsetree;
	List   *stmt_list;
	Node   *stmt;

	raw_parsetree_list = pg_parse_query(sql_string);

	if (list_length(raw_parsetree_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("plsample's DO block must have only one statement")));

	parsetree = (Node *) linitial(raw_parsetree_list);

	stmt_list = pg_analyze_and_rewrite(parsetree, sql_string, NULL, 0);

	stmt_list = pg_plan_queries(stmt_list, 0, NULL);

	if (list_length(stmt_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("plsample's DO block must have only one statement")));
			
	stmt = (Node *) linitial(stmt_list);
		
	if (!IsA(stmt, PlannedStmt) || ((PlannedStmt *) stmt)->utilityStmt != NULL)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("plsample's DO block must have only one statement")));

	return (PlannedStmt *) stmt;
}
