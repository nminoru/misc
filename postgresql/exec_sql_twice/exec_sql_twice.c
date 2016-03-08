/*-------------------------------------------------------------------------
 *
 * exec_sql_twice.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "c.h"
#include "executor/tstoreReceiver.h"
#include "executor/tuptable.h"
#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "nodes/pg_list.h"
#include "tcop/dest.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "utils/palloc.h"
#include "utils/snapmgr.h"
#include "utils/tuplestore.h"

typedef struct
{
	Tuplestorestate	   *tstore;
	TupleTableSlot	   *slot;
} sql_exec_twice_context_t;


static void sql_exec_twice_start(FuncCallContext *funcctx, text *sql);


PG_FUNCTION_INFO_V1(sql_exec_twice);
Datum
sql_exec_twice(PG_FUNCTION_ARGS)
{
	FuncCallContext	   *funcctx;
	sql_exec_twice_context_t *context;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext	oldcontext;
		text		   *sql;

		funcctx = SRF_FIRSTCALL_INIT();

		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		sql	= PG_GETARG_TEXT_P(0);

		sql_exec_twice_start(funcctx, sql);

		MemoryContextSwitchTo(oldcontext);
	}
	
	funcctx = SRF_PERCALL_SETUP();

	context = funcctx->user_fctx;

	if (tuplestore_gettupleslot(context->tstore, true /* forward */, false, context->slot))
	{
		HeapTuple	tuple;
		Datum		result;

		tuple = ExecCopySlotTuple(context->slot);
		result = HeapTupleGetDatum(tuple);
	
		SRF_RETURN_NEXT(funcctx, result);
	}
	else
	{
		tuplestore_end(context->tstore);

		SRF_RETURN_DONE(funcctx);
	}
}

static void
sql_exec_twice_start(FuncCallContext *funcctx, text *sql)
{
	sql_exec_twice_context_t	*context;
	const char	   *sql_str;
	DestReceiver   *dest;
	List		   *raw_parsetree_list;
	Node		   *parsetree;
	List		   *stmt_list;
	Node		   *stmt;
	QueryDesc	   *query_desc;

	context = palloc0(sizeof(sql_exec_twice_context_t));

	context->tstore = tuplestore_begin_heap(true, false, work_mem);

	sql_str = TextDatumGetCString(sql);

	dest = CreateDestReceiver(DestTuplestore);
	SetTuplestoreDestReceiverParams(dest, context->tstore, CurrentMemoryContext, false);

	raw_parsetree_list = pg_parse_query(sql_str);

	if (list_length(raw_parsetree_list) != 1)
		elog(ERROR, "aaa");

	parsetree = (Node *) linitial(raw_parsetree_list);

	stmt_list = pg_analyze_and_rewrite(parsetree,
									   sql_str,
									   NULL,
									   0);

	stmt_list = pg_plan_queries(stmt_list, 0, NULL);
		
	if (list_length(stmt_list) != 1)
		elog(ERROR, "bbb");
			
	stmt = (Node *) linitial(stmt_list);
		
	if (!IsA(stmt, PlannedStmt) || ((PlannedStmt *) stmt)->utilityStmt != NULL)
		elog(ERROR, "ccc");

	query_desc = CreateQueryDesc((PlannedStmt *) stmt,
								 sql_str,
								 GetActiveSnapshot(), NULL,
								 dest, NULL, 0);
				
	ExecutorStart(query_desc, 0);

	Assert(query_desc->tupDesc != NULL);

	funcctx->tuple_desc = CreateTupleDescCopy(query_desc->tupDesc);
	context->slot = MakeSingleTupleTableSlot(funcctx->tuple_desc);

	ExecutorRun(query_desc, ForwardScanDirection, 0L);
	ExecutorRewind(query_desc);
	ExecutorRun(query_desc, ForwardScanDirection, 0L);

	ExecutorFinish(query_desc);
	ExecutorEnd(query_desc);

	(*dest->rDestroy)(dest);

	FreeQueryDesc(query_desc);

	funcctx->user_fctx = context;
}
