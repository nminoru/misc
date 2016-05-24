#include "postgres.h"

#include "access/heapam.h"
#include "access/printtup.h"
#include "access/sdir.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "commands/trigger.h"
#include "executor/execdesc.h"
#include "executor/executor.h"
#include "executor/spi.h"
#include "fmgr.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "tcop/dest.h"
#include "tcop/pquery.h"
#include "tcop/tcopprot.h"
#include "utils/elog.h"
#include "utils/errcodes.h"
#include "utils/snapmgr.h"
#include "utils/syscache.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(plsample_call_handler);

Datum
plsample_call_handler(PG_FUNCTION_ARGS)
{
    Datum    retval = (Datum) 0;

	ereport(ERROR,
			(errcode(ERRCODE_SYNTAX_ERROR),
			 errmsg("plsample_call_handler")));

    return retval;
}

PG_FUNCTION_INFO_V1(plsample_inline_handler);

Datum
plsample_inline_handler(PG_FUNCTION_ARGS)
{
	InlineCodeBlock *codeblock = (InlineCodeBlock *) PG_GETARG_POINTER(0);
	List		   *raw_parsetree_list;
	Node		   *parsetree;
	List		   *stmt_list;
	Node		   *stmt;
	QueryDesc	   *query_desc;
	DestReceiver   *dest;

	raw_parsetree_list = pg_parse_query(codeblock->source_text);

	if (list_length(raw_parsetree_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("plsample's DO block must have only one statement")));

	parsetree = (Node *) linitial(raw_parsetree_list);

	stmt_list = pg_analyze_and_rewrite(parsetree, codeblock->source_text, NULL, 0);

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

	dest = CreateDestReceiver(DestRemote);
	SetRemoteDestReceiverParams(dest, ActivePortal);

	query_desc = CreateQueryDesc((PlannedStmt *) stmt,
								 codeblock->source_text,
								 GetActiveSnapshot(), InvalidSnapshot,
								 dest, NULL, 0);

	ExecutorStart(query_desc, 0);

	ExecutorRun(query_desc, ForwardScanDirection, 0L);

	ExecutorFinish(query_desc);
	ExecutorEnd(query_desc);

	FreeQueryDesc(query_desc);

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(plsample_validator);

Datum
plsample_validator(PG_FUNCTION_ARGS)
{
	Oid funcoid = PG_GETARG_OID(0);

	ereport(ERROR,
			(errcode(ERRCODE_SYNTAX_ERROR),
			 errmsg("plsample_validator: oid=%d", funcoid)));

	PG_RETURN_VOID();
}
