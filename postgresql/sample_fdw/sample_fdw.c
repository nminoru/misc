/*-------------------------------------------------------------------------
 * sample_fdw.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/heapam.h"
#include "access/htup.h"
#include "access/htup_details.h"
#include "access/reloptions.h"
#include "access/relscan.h"
#include "access/sdir.h"
#include "c.h"
#include "catalog/pg_attribute.h"
#include "commands/defrem.h"
#include "commands/explain.h"
#include "commands/explain.h"
#include "executor/executor.h"
#include "executor/tuptable.h"
#include "fmgr.h"
#include "foreign/fdwapi.h"
#include "miscadmin.h"
#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"
#include "optimizer/cost.h"
#include "optimizer/pathnode.h"
#include "optimizer/planmain.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/var.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/errcodes.h"
#include "utils/palloc.h"
#include "utils/rel.h"
#include "utils/relcache.h"

#include "catalog/namespace.h"

#include "sample_fdw.h"

#ifndef SizeofHeapTupleHeader
#define SizeofHeapTupleHeader offsetof(HeapTupleHeaderData, t_bits)
#endif

#if PG_VERSION_NUM < 90500
#error "sample_fdw supports PostgreSQL 9.5 or newer versions."
#endif

PG_MODULE_MAGIC;

/*
 * FDW callback routines
 */
static void sampleFdwGetForeignRelSize(PlannerInfo *root,
									   RelOptInfo *baserel,
									   Oid foreigntableid);
static List *charToQualifiedNameList(const char *name);
static void sampleFdwGetForeignPaths(PlannerInfo *root,
									 RelOptInfo *baserel,
									 Oid foreigntableid);
static ForeignScan *sampleFdwGetForeignPlan(PlannerInfo *root,
											RelOptInfo *baserel,
											Oid foreigntableid,
											ForeignPath *best_path,
											List *tlist,
											List *scan_clauses,
											Plan *outer_plan);
static void sampleFdwBeginForeignScan(ForeignScanState *node, int eflags);
static TupleTableSlot *sampleFdwIterateForeignScan(ForeignScanState *node);
static void sampleFdwReScanForeignScan(ForeignScanState *node);
static void sampleFdwEndForeignScan(ForeignScanState *node);
static void sampleFdwAddForeignUpdateTargets(Query *parsetree,
											 RangeTblEntry *target_rte,
											 Relation target_relation);
static List *sampleFdwPlanForeignModify(PlannerInfo *root,
										ModifyTable *plan,
										Index resultRelation,
										int subplan_index);
static void sampleFdwBeginForeignModify(ModifyTableState *mtstate,
										ResultRelInfo *resultRelInfo,
										List *fdw_private,
										int subplan_index,
										int eflags);
static TupleTableSlot *sampleFdwExecForeignInsert(EState *estate,
												  ResultRelInfo *resultRelInfo,
												  TupleTableSlot *slot,
												  TupleTableSlot *planSlot);
static TupleTableSlot *sampleFdwExecForeignUpdate(EState *estate,
												  ResultRelInfo *resultRelInfo,
												  TupleTableSlot *slot,
												  TupleTableSlot *planSlot);
static TupleTableSlot *sampleFdwExecForeignDelete(EState *estate,
												  ResultRelInfo *resultRelInfo,
												  TupleTableSlot *slot,
												  TupleTableSlot *planSlot);
static void sampleFdwEndForeignModify(EState *estate,
									  ResultRelInfo *resultRelInfo);
static int	sampleFdwIsForeignRelUpdatable(Relation rel);
static void sampleFdwExplainForeignScan(ForeignScanState *node,
										ExplainState *es);
static void sampleFdwExplainForeignModify(ModifyTableState *mtstate,
										  ResultRelInfo *rinfo,
										  List *fdw_private,
										  int subplan_index,
										  ExplainState *es);
static bool sampleFdwAnalyzeForeignTable(Relation relation,
										 AcquireSampleRowsFunc *func,
										 BlockNumber *totalpages);
static List *sampleFdwImportForeignSchema(ImportForeignSchemaStmt *stmt,
										  Oid serverOid);


PG_FUNCTION_INFO_V1(sample_fdw_handler);
Datum
sample_fdw_handler(PG_FUNCTION_ARGS)
{
	FdwRoutine *routine = makeNode(FdwRoutine);

	/* Functions for scanning foreign tables */
	routine->GetForeignRelSize = sampleFdwGetForeignRelSize;
	routine->GetForeignPaths = sampleFdwGetForeignPaths;
	routine->GetForeignPlan = sampleFdwGetForeignPlan;
	routine->BeginForeignScan = sampleFdwBeginForeignScan;
	routine->IterateForeignScan = sampleFdwIterateForeignScan;
	routine->ReScanForeignScan = sampleFdwReScanForeignScan;
	routine->EndForeignScan = sampleFdwEndForeignScan;

	/* Functions for updating foreign tables */
	routine->AddForeignUpdateTargets = sampleFdwAddForeignUpdateTargets;
	routine->PlanForeignModify = sampleFdwPlanForeignModify;
	routine->BeginForeignModify = sampleFdwBeginForeignModify;
	routine->ExecForeignInsert = sampleFdwExecForeignInsert;
	routine->ExecForeignUpdate = sampleFdwExecForeignUpdate;
	routine->ExecForeignDelete = sampleFdwExecForeignDelete;
	routine->EndForeignModify = sampleFdwEndForeignModify;
	routine->IsForeignRelUpdatable = sampleFdwIsForeignRelUpdatable;

	/* Support functions for EXPLAIN */
	routine->ExplainForeignScan = sampleFdwExplainForeignScan;
	routine->ExplainForeignModify = sampleFdwExplainForeignModify;

	/* Support functions for ANALYZE */
	routine->AnalyzeForeignTable = sampleFdwAnalyzeForeignTable;

	/* Support functions for IMPORT FOREIGN SCHEMA */
	routine->ImportForeignSchema = sampleFdwImportForeignSchema;

	PG_RETURN_POINTER(routine);
}

PG_FUNCTION_INFO_V1(sample_fdw_validator);
Datum
sample_fdw_validator(PG_FUNCTION_ARGS)
{
	List   *options_list;
	Oid		catalog;

	options_list = untransformRelOptions(PG_GETARG_DATUM(0));
	catalog = PG_GETARG_OID(1);

	/* @todo */

	PG_RETURN_VOID();
}

static void
sampleFdwGetForeignRelSize(PlannerInfo *root,
					  RelOptInfo *baserel,
					  Oid foreigntableid)
{
	SampleFdwRelationInfo *fdw_rel_info;
	Oid table_oid = (Oid) 0;
	List *quals = NIL;
	ListCell *lc;

	fdw_rel_info = (SampleFdwRelationInfo *) palloc0(sizeof(SampleFdwRelationInfo));

	fdw_rel_info->table = GetForeignTable(foreigntableid);
	fdw_rel_info->server = GetForeignServer(fdw_rel_info->table->serverid);

	fdw_rel_info->fdw_startup_cost = DEFAULT_FDW_STARTUP_COST;
	fdw_rel_info->fdw_tuple_cost = DEFAULT_FDW_TUPLE_COST;

#if 0
	foreach(lc, fdw_rel_info->server->options)
	{

	}
#endif

	foreach(lc, fdw_rel_info->table->options)
	{
		DefElem	   *def = (DefElem *) lfirst(lc);

		if (strcmp(def->defname, "table_name") == 0)
		{
			RangeVar   *relvar;

			relvar = makeRangeVarFromNameList(charToQualifiedNameList(defGetString(def)));
			table_oid = RangeVarGetRelid(relvar, AccessShareLock, false);
		}
	}

	if (table_oid == 0)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("fff")));

	fdw_rel_info->remote_tableoid = table_oid;
	fdw_rel_info->attrs_used = NULL;

	pull_varattnos((Node *) baserel->reltargetlist, baserel->relid,
				   &fdw_rel_info->attrs_used);

	baserel->fdw_private = (void *) fdw_rel_info;

	foreach(lc, baserel->baserestrictinfo)
	{
		RestrictInfo *ri = (RestrictInfo *) lfirst(lc);

		pull_varattnos((Node *) ri->clause, baserel->relid, &fdw_rel_info->attrs_used);

		quals = lappend(quals, ri);
	}

	fdw_rel_info->qual_selectivity =
		clauselist_selectivity(root,
							   quals,
							   baserel->relid,
							   JOIN_INNER,
							   NULL);

	cost_qual_eval(&fdw_rel_info->qual_cost, quals, root);

	if (baserel->pages == 0 && baserel->tuples == 0)
	{
		baserel->pages = 10;
		baserel->tuples = 
			(10 * BLCKSZ) / (baserel->width + MAXALIGN(SizeofHeapTupleHeader));
	}

	set_baserel_size_estimates(root, baserel);

#if 0
	estimate_path_cost_size(root, baserel, NIL,
							&fdw_rel_info->rows, &fdw_rel_info->width,
							&fdw_rel_info->startup_cost, &fdw_rel_info->total_cost);
#endif

}

static List *
charToQualifiedNameList(const char *name)
{
	char	   *rawname;
	List       *result = NIL;
	List       *namelist;
	ListCell   *l;

	rawname = pstrdup(name);

	if (!SplitIdentifierString(rawname, '.', &namelist))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_NAME),
				 errmsg("invalid name syntax")));

	if (namelist == NIL)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_NAME),
                 errmsg("invalid name syntax")));

	foreach(l, namelist)
	{
		char       *curname = (char *) lfirst(l);

		result = lappend(result, makeString(pstrdup(curname)));
	}

	pfree(rawname);
    list_free(namelist);

	return result;
}

static void
sampleFdwGetForeignPaths(PlannerInfo *root,
					RelOptInfo *baserel,
					Oid foreigntableid)
{
	SampleFdwRelationInfo *fdw_rel_info = (SampleFdwRelationInfo *) baserel->fdw_private;
	ForeignPath *path;
	List	   *ppi_list;
	ListCell   *lc;

	path = create_foreignscan_path(root, baserel,
								   fdw_rel_info->rows,
								   fdw_rel_info->startup_cost,
								   fdw_rel_info->total_cost,
								   NIL,		/* no pathkeys */
								   NULL,	/* no require_outer */
								   NULL,	/* no outer rel either */
								   NIL);	/* no fdw_private list */

	add_path(baserel, (Path *) path);
}

static ForeignScan *
sampleFdwGetForeignPlan(PlannerInfo *root,
						RelOptInfo *baserel,
						Oid foreigntableid,
						ForeignPath *best_path,
						List *tlist,
						List *scan_clauses,
						Plan *outer_plan)
{
	SampleFdwRelationInfo *fdw_rel_info = (SampleFdwRelationInfo *) baserel->fdw_private;
	Index		scan_relid = baserel->relid;
	List	   *fdw_exprs = NIL;
	List	   *fdw_private = NIL;
	List	   *fdw_scan_tlist = NIL;
	List	   *fdw_recheck_quals = NIL;

	fdw_private = list_make1_oid(fdw_rel_info->remote_tableoid);

	scan_clauses = extract_actual_clauses(scan_clauses, false);

	return make_foreignscan(tlist,
							scan_clauses,
							scan_relid,
							fdw_exprs,
							fdw_private,
							fdw_scan_tlist,
							fdw_recheck_quals,
							outer_plan);
}

static void
sampleFdwBeginForeignScan(ForeignScanState *node, int eflags)
{
	ForeignScan *fsplan = (ForeignScan *) node->ss.ps.plan;
	EState	   *estate = node->ss.ps.state;
	SampleFdwScanState *inner_state;
	TupleDesc	fdwTupleDesc;
	AttrNumber	fdwAttrNum;
	TupleDesc	remoteTupleDesc;
	int			last_remote_var = 0;

	if (eflags & EXEC_FLAG_EXPLAIN_ONLY)
		return;

	inner_state = (SampleFdwScanState *) palloc0(sizeof(SampleFdwScanState));
	node->fdw_state = (void *) inner_state;

	inner_state->remote_tableoid = linitial_oid(fsplan->fdw_private);

	inner_state->remote_relation = heap_open(inner_state->remote_tableoid, AccessShareLock);
	inner_state->remote_scandesc = heap_beginscan(inner_state->remote_relation,
												  estate->es_snapshot,
												  0,
												  NULL);

	inner_state->remote_tts = MakeSingleTupleTableSlot(RelationGetDescr(inner_state->remote_relation));

	/*
	 *
	 */
	fdwTupleDesc = RelationGetDescr(node->ss.ss_currentRelation);
	remoteTupleDesc = RelationGetDescr(inner_state->remote_relation);

	inner_state->attr_map = palloc0(sizeof(AttrNumber) * fdwTupleDesc->natts);

	for (fdwAttrNum = 1 ; fdwAttrNum <= fdwTupleDesc->natts; fdwAttrNum++)
	{
		Form_pg_attribute attr = fdwTupleDesc->attrs[fdwAttrNum - 1];
		List	   *options;
		ListCell   *lc;
		const char *column_name;
		AttrNumber	remoteAttrNum;

		if (attr->attisdropped)
			continue;

		column_name = NameStr(attr->attname);

		options = GetForeignColumnOptions(inner_state->remote_tableoid, fdwAttrNum);

		foreach(lc, options)
		{
			DefElem	   *def = (DefElem *) lfirst(lc);

			if (strcmp(def->defname, "column_name") == 0)
			{
				column_name = defGetString(def);
				break;
			}
		}

		for (remoteAttrNum = 1 ; remoteAttrNum <= remoteTupleDesc->natts; remoteAttrNum++)
		{
			Form_pg_attribute remoteAttr = remoteTupleDesc->attrs[remoteAttrNum - 1];
			
			if (strcmp(column_name, NameStr(remoteAttr->attname)) == 0)
			{
				inner_state->attr_map[fdwAttrNum - 1] = remoteAttrNum;

				if (last_remote_var < remoteAttrNum)
					last_remote_var = remoteAttrNum;
				break;
			}
		}
	}

	inner_state->last_remote_var = last_remote_var;
}

static TupleTableSlot *
sampleFdwIterateForeignScan(ForeignScanState *node)
{
	SampleFdwScanState *inner_state;
	HeapTuple tuple;
	HeapScanDesc scandesc;
	TupleTableSlot *scan_tts;
	TupleTableSlot *remote_tts;

	inner_state = node->fdw_state;

	if (inner_state == NULL)
		return NULL;

	scandesc = inner_state->remote_scandesc;

	remote_tts = inner_state->remote_tts;
	scan_tts = node->ss.ss_ScanTupleSlot;

	tuple = heap_getnext(scandesc, ForwardScanDirection);
	
	if (tuple)
	{
		int	natts;	
		AttrNumber attrnum;

		ExecStoreTuple(tuple, remote_tts, scandesc->rs_cbuf, false);
		ExecClearTuple(scan_tts);

		slot_getsomeattrs(remote_tts, inner_state->last_remote_var);

		natts = RelationGetDescr(node->ss.ss_currentRelation)->natts;

		for (attrnum = 1 ; attrnum <= natts ; attrnum++)
		{
			int i = inner_state->attr_map[attrnum - 1];

			if (i != 0)
			{
				scan_tts->tts_values[attrnum - 1] = remote_tts->tts_values[i];
				scan_tts->tts_isnull[attrnum - 1] = remote_tts->tts_isnull[i];
			}
			else
			{
				scan_tts->tts_values[attrnum - 1] = (Datum) 0;
				scan_tts->tts_isnull[attrnum - 1] = false;
			}
		}

		ExecStoreVirtualTuple(scan_tts);
	}
	else
	{
		ExecClearTuple(remote_tts);
		ExecClearTuple(scan_tts);
	}

	return scan_tts;
}

static void
sampleFdwReScanForeignScan(ForeignScanState *node)
{
	SampleFdwScanState *inner_state;

	inner_state = node->fdw_state;

	if (inner_state == NULL)
		return;

	heap_rescan(inner_state->remote_scandesc, NULL);
}

static void
sampleFdwEndForeignScan(ForeignScanState *node)
{
	SampleFdwScanState *inner_state;

	inner_state = node->fdw_state;

	if (inner_state == NULL)
		return;

	ExecDropSingleTupleTableSlot(inner_state->remote_tts);

	heap_endscan(inner_state->remote_scandesc);
	heap_close(inner_state->remote_relation, NoLock);
}

static void
sampleFdwAddForeignUpdateTargets(Query *parsetree,
								 RangeTblEntry *target_rte,
								 Relation target_relation)
{
}

static List *
sampleFdwPlanForeignModify(PlannerInfo *root,
						   ModifyTable *plan,
						   Index resultRelation,
						   int subplan_index)
{
	return NIL;
}

static void
sampleFdwBeginForeignModify(ModifyTableState *mtstate,
							ResultRelInfo *resultRelInfo,
							List *fdw_private,
							int subplan_index,
							int eflags)
{
}

static TupleTableSlot *
sampleFdwExecForeignInsert(EState *estate,
						   ResultRelInfo *resultRelInfo,
						   TupleTableSlot *slot,
						   TupleTableSlot *planSlot)
{
	return NULL;
}

static TupleTableSlot *
sampleFdwExecForeignUpdate(EState *estate,
						   ResultRelInfo *resultRelInfo,
						   TupleTableSlot *slot,
						   TupleTableSlot *planSlot)
{
	return NULL;
}

static TupleTableSlot *
sampleFdwExecForeignDelete(EState *estate,
						   ResultRelInfo *resultRelInfo,
						   TupleTableSlot *slot,
						   TupleTableSlot *planSlot)
{
	return NULL;
}

static void
sampleFdwEndForeignModify(EState *estate,
						  ResultRelInfo *resultRelInfo)
{
}

static int
sampleFdwIsForeignRelUpdatable(Relation rel)
{
	return 0;
}

static void
sampleFdwExplainForeignScan(ForeignScanState *node,
							ExplainState *es)
{
	ForeignScan *fsplan = (ForeignScan *) node->ss.ps.plan;
	Oid tableoid;

	tableoid = linitial_oid(fsplan->fdw_private);

	ExplainPropertyInteger("Remote Table Oid", tableoid, es);
}

static void
sampleFdwExplainForeignModify(ModifyTableState *mtstate,
						 ResultRelInfo *rinfo,
						 List *fdw_private,
						 int subplan_index,
						 ExplainState *es)
{
}

static bool
sampleFdwAnalyzeForeignTable(Relation relation,
						AcquireSampleRowsFunc *func,
						BlockNumber *totalpages)
{
	return false;
}

static List *
sampleFdwImportForeignSchema(ImportForeignSchemaStmt *stmt,
						Oid serverOid)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("ImportForeignSchema callback does not implemented")));
}
