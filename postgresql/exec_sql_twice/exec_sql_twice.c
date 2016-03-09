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
#include "nodes/execnodes.h"


static void sql_exec_twice_start(text *sql, Tuplestorestate *tstore, TupleDesc outerTupdesc);
static bool equalTupleDescsIgnoringAttName(TupleDesc tupdesc1, TupleDesc tupdesc2);

PG_MODULE_MAGIC;


PG_FUNCTION_INFO_V1(sql_exec_twice);
Datum
sql_exec_twice(PG_FUNCTION_ARGS)
{
	text		   *sql;
	ReturnSetInfo  *rsi;
	TupleDesc		tupdesc;
	MemoryContext	oldcontext;

	sql	= PG_GETARG_TEXT_P(0);

	rsi = (ReturnSetInfo *) fcinfo->resultinfo;

	if (!rsi || !IsA(rsi, ReturnSetInfo) ||
		(rsi->allowedModes & SFRM_Materialize) == 0 ||
		rsi->expectedDesc == NULL)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that "
						"cannot accept a set")));

	rsi->returnMode = SFRM_Materialize;

	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("function returning record called in context "
						"that cannot accept type record")));

	oldcontext = MemoryContextSwitchTo(rsi->econtext->ecxt_per_query_memory);

	rsi->setDesc = CreateTupleDescCopy(tupdesc);
	BlessTupleDesc(rsi->setDesc);
	rsi->setResult = tuplestore_begin_heap(rsi->allowedModes & SFRM_Materialize_Random, false, work_mem);

	MemoryContextSwitchTo(oldcontext);

	sql_exec_twice_start(sql, rsi->setResult, rsi->setDesc);

	PG_RETURN_NULL();
}

static void
sql_exec_twice_start(text *sql, Tuplestorestate *tstore, TupleDesc outerTupdesc)
{
	const char	   *sql_str;
	DestReceiver   *dest;
	List		   *raw_parsetree_list;
	Node		   *parsetree;
	List		   *stmt_list;
	Node		   *stmt;
	QueryDesc	   *query_desc;

	sql_str = TextDatumGetCString(sql);

	dest = CreateDestReceiver(DestTuplestore);
	SetTuplestoreDestReceiverParams(dest, tstore, CurrentMemoryContext, false);

	raw_parsetree_list = pg_parse_query(sql_str);

	if (list_length(raw_parsetree_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("sql_exec_twice() must have only one statement")));

	parsetree = (Node *) linitial(raw_parsetree_list);

	stmt_list = pg_analyze_and_rewrite(parsetree,
									   sql_str,
									   NULL,
									   0);

	stmt_list = pg_plan_queries(stmt_list, 0, NULL);
		
	if (list_length(stmt_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("sql_exec_twice() must have only one statement")));
			
	stmt = (Node *) linitial(stmt_list);
		
	if (!IsA(stmt, PlannedStmt) || ((PlannedStmt *) stmt)->utilityStmt != NULL)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("sql_exec_twice() must have only one statement")));

	query_desc = CreateQueryDesc((PlannedStmt *) stmt,
								 sql_str,
								 GetActiveSnapshot(), NULL,
								 dest, NULL, 0);
				
	ExecutorStart(query_desc, 0);

	if (!equalTupleDescsIgnoringAttName(query_desc->tupDesc, outerTupdesc))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_FUNCTION_DEFINITION),
				 errmsg("set-valued function called in context that cannot accept a set")));

	ExecutorRun(query_desc, ForwardScanDirection, 0L);
	ExecutorRewind(query_desc);
	ExecutorRun(query_desc, ForwardScanDirection, 0L);

	ExecutorFinish(query_desc);
	ExecutorEnd(query_desc);

	(*dest->rDestroy)(dest);

	FreeQueryDesc(query_desc);
}

static bool 
equalTupleDescsIgnoringAttName(TupleDesc tupdesc1, TupleDesc tupdesc2)
{
	int			i;

	if (tupdesc1->natts != tupdesc2->natts)
		return false;
	if (tupdesc1->tdtypeid != tupdesc2->tdtypeid)
		return false;
	if (tupdesc1->tdhasoid != tupdesc2->tdhasoid)
		return false;

	for (i = 0; i < tupdesc1->natts; i++)
	{
		Form_pg_attribute attr1 = tupdesc1->attrs[i];
		Form_pg_attribute attr2 = tupdesc2->attrs[i];

#if 0
		if (strcmp(NameStr(attr1->attname), NameStr(attr2->attname)) != 0)
			return false;
#endif

		if (attr1->atttypid != attr2->atttypid)
			return false;
		if (attr1->attstattarget != attr2->attstattarget)
			return false;
		if (attr1->attlen != attr2->attlen)
			return false;
		if (attr1->attndims != attr2->attndims)
			return false;
		if (attr1->atttypmod != attr2->atttypmod)
			return false;
		if (attr1->attbyval != attr2->attbyval)
			return false;
		if (attr1->attstorage != attr2->attstorage)
			return false;
		if (attr1->attalign != attr2->attalign)
			return false;
		if (attr1->attnotnull != attr2->attnotnull)
			return false;
		if (attr1->atthasdef != attr2->atthasdef)
			return false;
		if (attr1->attisdropped != attr2->attisdropped)
			return false;
		if (attr1->attislocal != attr2->attislocal)
			return false;
		if (attr1->attinhcount != attr2->attinhcount)
			return false;
		if (attr1->attcollation != attr2->attcollation)
			return false;
		/* attacl, attoptions and attfdwoptions are not even present... */
	}

	return true;
}
