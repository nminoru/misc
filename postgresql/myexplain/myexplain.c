/*-------------------------------------------------------------------------
 *
 * myexplain.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/tupdesc.h"
#include "catalog/pg_type.h"
#include "commands/explain.h"
#include "executor/executor.h"
#include "fmgr.h"
#include "nodes/pg_list.h"
#include "tcop/dest.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/palloc.h"
#include "utils/snapmgr.h"
#include "utils/elog.h"
#include "utils/errcodes.h"


PG_MODULE_MAGIC;

static PlannedStmt *translate_sql(const char *sql_str);

PG_FUNCTION_INFO_V1(myexplain);
Datum
myexplain(PG_FUNCTION_ARGS)
{
	text		   *sql;
	const char	   *sql_str;
	ExplainState	es;
	DestReceiver   *dest = None_Receiver;
	QueryDesc	   *queryDesc;
	PlannedStmt	   *plannedstmt;
	TupleDesc		tupdesc;
	TupOutputState *tstate;
	text		   *output;

	sql	= PG_GETARG_TEXT_P(0);

	sql_str = TextDatumGetCString(sql);
	
	ExplainInitState(&es);

	es.analyze	= false;
	es.verbose	= true;
	es.costs	= false;
	es.buffers	= false;
	es.timing	= false;
	es.format	= EXPLAIN_FORMAT_TEXT;

	ExplainBeginOutput(&es);

	plannedstmt = translate_sql(sql_str);

	queryDesc = CreateQueryDesc(plannedstmt, sql_str,
								GetActiveSnapshot(), InvalidSnapshot,
								dest, NULL, 0);

	ExecutorStart(queryDesc, EXEC_FLAG_EXPLAIN_ONLY);

	ExplainPrintPlan(&es, queryDesc);
	ExecutorEnd(queryDesc);
	FreeQueryDesc(queryDesc);

	ExplainEndOutput(&es);

	tupdesc = CreateTemplateTupleDesc(1, false);
	TupleDescInitEntry(tupdesc, (AttrNumber) 1, "QUERY PLAN", TEXTOID, -1, 0);
	tstate = begin_tup_output_tupdesc(dest, tupdesc);
	do_text_output_multiline(tstate, es.str->data);
	output = cstring_to_text(es.str->data);
	pfree(es.str->data);

	PG_RETURN_TEXT_P(output);
}

static PlannedStmt *
translate_sql(const char *sql_str)
{
	List   *raw_parsetree_list;
	Node   *parsetree;
	List   *stmt_list;

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
			
	return (PlannedStmt *) linitial(stmt_list);
}

