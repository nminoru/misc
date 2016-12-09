/*-------------------------------------------------------------------------
 * whole_row_var_arg.c
 *
 * Copyright (c) 2016,2017 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/heapam.h"
#include "access/htup.h"
#include "access/htup_details.h"
#include "access/sdir.h"
#include "access/transam.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "fmgr.h"
#include "funcapi.h"
#include "lib/stringinfo.h"
#include "nodes/execnodes.h"
#include "nodes/makefuncs.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "optimizer/planner.h"
#include "storage/lock.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "utils/relcache.h"
#include "utils/snapmgr.h"

PG_MODULE_MAGIC;


static Datum whole_row_var_arg_core(FuncExprState *fcache, ExprContext *econtext, bool *isNull, ExprDoneCond *isDone);


typedef struct
{
	PlanState  *planstate;
} whole_row_var_arg_state;


PG_FUNCTION_INFO_V1(whole_row_var_arg);
Datum
whole_row_var_arg(PG_FUNCTION_ARGS)
{
	FuncExprState *funcexpr;
	WholeRowVarExprState *wstate;
	whole_row_var_arg_state *state;

	if (fcinfo->flinfo->fn_extra ||!fcinfo->flinfo->fn_expr || !IsA(fcinfo->flinfo->fn_expr, FuncExpr))
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("should not reach here")));

	funcexpr = (FuncExprState *) ((char *) fcinfo - offsetof(FuncExprState, fcinfo_data));

	if (!IsA(funcexpr, FuncExprState) || list_length(funcexpr->args) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("should not reach here")));

	wstate = (WholeRowVarExprState *) list_nth(funcexpr->args, 1);

	if (!IsA(wstate, WholeRowVarExprState) || (wstate->parent == NULL))
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("ERRCODE_INTERNAL_ERROR")));

	state = MemoryContextAlloc(fcinfo->flinfo->fn_mcxt, sizeof(whole_row_var_arg_state));
		
	state->planstate = wstate->parent;
	fcinfo->flinfo->fn_extra = state;

	funcexpr->xprstate.evalfunc = (ExprStateEvalFunc) whole_row_var_arg_core;

	whole_row_var_arg_core(funcexpr, NULL, &fcinfo->isnull, NULL);

	PG_RETURN_NULL();
}


static Datum
whole_row_var_arg_core(FuncExprState *fcache, ExprContext *econtext, bool *isNull, ExprDoneCond *isDone)
{
	whole_row_var_arg_state *state;
	TupleTableSlot *tts;
	TupleDesc tupDesc;
	int i;

	state = fcache->fcinfo_data.flinfo->fn_extra;

	tts = state->planstate->ps_ResultTupleSlot;

	tupDesc = tts->tts_tupleDescriptor;

	for (i = 0 ; i < tupDesc->natts - 1; i++)
	{
		Datum datum;

		if (tts->tts_isnull[i])
			continue;

		datum = tts->tts_values[i];

		switch (tupDesc->attrs[i]->atttypid)
		{
			case INT4OID:
				tts->tts_values[i] = Int32GetDatum(DatumGetInt32(datum) + 1);
				break;

			case TEXTOID:
				{
					StringInfoData str;
					char *body;

					initStringInfo(&str);

					body = text_to_cstring((const text *) DatumGetPointer(datum));

					appendStringInfoString(&str, body);

					pfree(body);

					appendStringInfoString(&str, "hoge");

					tts->tts_values[i] = PointerGetDatum(cstring_to_text_with_len(str.data, str.len));
				}
				break;

			case FLOAT8OID:
				tts->tts_values[i] = Float8GetDatum(DatumGetFloat8(datum) + 1.0);
				break;

			default:
				break;
		}
	}

	*isNull = true;

	return (Datum) 0;
}
