/*-------------------------------------------------------------------------
 *
 * diskstats.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include <stdio.h>

#include "access/tupdesc.h"
#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "utils/elog.h"
#include "utils/errcodes.h"
#include "utils/palloc.h"
#include "utils/tuplestore.h"
#include "executor/tuptable.h"
#include "utils/builtins.h"


static bool get_diskstat_entry(FILE *file, TupleTableSlot *tts);


PG_MODULE_MAGIC;


PG_FUNCTION_INFO_V1(show_diskstats1);
Datum
show_diskstats1(PG_FUNCTION_ARGS)
{
    FuncCallContext  *funcctx;
    Datum             result;
	struct
	{
		FILE	   *file;
	} *diskstat_ctx;

	if (SRF_IS_FIRSTCALL())
	{
		/* show_diskstats1() */
		MemoryContext oldcontext;
		TupleDesc	  tupleDesc;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		if (get_call_result_type(fcinfo, NULL, &tupleDesc) != TYPEFUNC_COMPOSITE)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("function returning record called in context "
							"that cannot accept type record")));

		diskstat_ctx = palloc0(sizeof(*diskstat_ctx));
		diskstat_ctx->file = fopen("/proc/diskstats", "r");

		if (diskstat_ctx->file == NULL)
			ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("could not open file \"/proc/diskstats\" for writing: %m")));

		funcctx->user_fctx = diskstat_ctx;
		funcctx->tuple_desc = tupleDesc;
		funcctx->slot = MakeSingleTupleTableSlot(tupleDesc);

		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	diskstat_ctx = funcctx->user_fctx;

	if (get_diskstat_entry(diskstat_ctx->file, funcctx->slot))
	{
		HeapTuple tuple;

		tuple = ExecCopySlotTuple(funcctx->slot);
		result = HeapTupleGetDatum(tuple);

		SRF_RETURN_NEXT(funcctx, result);
	}
	else
	{
		fclose(diskstat_ctx->file);

		SRF_RETURN_DONE(funcctx);
	}
}

PG_FUNCTION_INFO_V1(show_diskstats2);
Datum
show_diskstats2(PG_FUNCTION_ARGS)
{
	ReturnSetInfo  *rsi;
	TupleDesc		tupdesc;
	TupleTableSlot *tts;
	MemoryContext	oldcontext;

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
	tts = MakeSingleTupleTableSlot(rsi->setDesc);

	{
		FILE *file;

		file = fopen("/proc/diskstats", "r");
		if (file == NULL)
			ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("could not open file \"/proc/diskstats\" for writing: %m")));

		while (get_diskstat_entry(file, tts))
			tuplestore_puttupleslot(rsi->setResult, tts);

		fclose(file);
	}

	MemoryContextSwitchTo(oldcontext);

	PG_RETURN_NULL();
}

static bool
get_diskstat_entry(FILE *file, TupleTableSlot *tts)
{
	char buffer[128];
	char diskname[128];
	int ret;
	int major, minor, read_time, write_time, io, io_time, weighted_io_time;
	long read_completed, read_merged, read_sectors, write_completed, write_merged, write_sectors;

retry:
	if (fgets(buffer, sizeof(buffer), file) == NULL)
		return false;

	ret = sscanf(buffer, "%d %d %s %lu %lu %lu %u %lu %lu %lu %u %u %u %u",
				 &major, &minor, diskname,
				 &read_completed, &read_merged, &read_sectors,
				 &read_time,
				 &write_completed, &write_merged, &write_sectors,
				 &write_time,
				 &io,
				 &io_time,
				 &weighted_io_time);

	if (ret != 14)
		goto retry;

	ExecClearTuple(tts);

	tts->tts_values[ 0] = Int32GetDatum(major);
	tts->tts_isnull[ 0] = false;
	tts->tts_values[ 1] = Int32GetDatum(minor);
	tts->tts_isnull[ 1] = false;
	tts->tts_values[ 2] = PointerGetDatum(cstring_to_text(diskname));
	tts->tts_isnull[ 2] = false;
	tts->tts_values[ 3] = Int64GetDatum(read_completed);
	tts->tts_isnull[ 3] = false;
	tts->tts_values[ 4] = Int64GetDatum(read_merged);
	tts->tts_isnull[ 4] = false;
	tts->tts_values[ 5] = Int64GetDatum(read_sectors);
	tts->tts_isnull[ 5] = false;
	tts->tts_values[ 6] = Int32GetDatum(read_time);
	tts->tts_isnull[ 6] = false;
	tts->tts_values[ 7] = Int64GetDatum(write_completed);
	tts->tts_isnull[ 7] = false;
	tts->tts_values[ 8] = Int64GetDatum(write_merged);
	tts->tts_isnull[ 8] = false;
	tts->tts_values[ 9] = Int64GetDatum(write_sectors);
	tts->tts_isnull[ 9] = false;
	tts->tts_values[10] = Int32GetDatum(write_time);
	tts->tts_isnull[10] = false;
	tts->tts_values[11] = Int32GetDatum(io);
	tts->tts_isnull[11] = false;
	tts->tts_values[12] = Int32GetDatum(io_time);
	tts->tts_isnull[12] = false;
	tts->tts_values[13] = Int32GetDatum(weighted_io_time);
	tts->tts_isnull[13] = false;

	ExecStoreVirtualTuple(tts);

	return true;
}


