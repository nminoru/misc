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
#include "catalog/namespace.h"
#include "catalog/pg_attribute.h"
#include "commands/defrem.h"
#include "commands/explain.h"
#include "executor/executor.h"
#include "executor/tuptable.h"
#include "fmgr.h"
#include "foreign/fdwapi.h"
#include "miscadmin.h"
#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/params.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"
#include "optimizer/cost.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/planmain.h"
#include "optimizer/planner.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/var.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/errcodes.h"
#include "utils/lsyscache.h"
#include "utils/palloc.h"
#include "utils/rel.h"
#include "utils/relcache.h"
#include "utils/syscache.h"

#include "sample_fdw.h"

#ifndef SizeofHeapTupleHeader
#define SizeofHeapTupleHeader offsetof(HeapTupleHeaderData, t_bits)
#endif

#if PG_VERSION_NUM < 90500
#error "sample_fdw supports PostgreSQL 9.5 or newer versions."
#endif

PG_MODULE_MAGIC;

extern void _PG_init(void);

static Oid sample_fdw_oid;
static bool use_sample_fdw;

static planner_hook_type planner_hook_prev;
static set_join_pathlist_hook_type set_join_pathlist_hook_prev;

static PlannedStmt *sample_fdw_palnner(Query *parse, int cursorOptions, ParamListInfo boundParams);
static void sample_fdw_set_join_pathlist(PlannerInfo *root,
										 RelOptInfo *joinrel,
										 RelOptInfo *outerrel,
										 RelOptInfo *innerrel,
										 JoinType jointype,
										 JoinPathExtraData *extra);

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
static void sampleFdwGetForeignJoinPaths(PlannerInfo *root,
										 RelOptInfo *joinrel,
										 RelOptInfo *outerrel,
										 RelOptInfo *innerrel,
										 JoinType jointype,
										 JoinPathExtraData *extra);
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
static RowMarkType sampleFdwGetForeignRowMarkType(RangeTblEntry *rte,
												  LockClauseStrength strength);
static HeapTuple sampleFdwRefetchForeignRow(EState *estate,
											ExecRowMark *erm,
											Datum rowid,
											bool *updated);
static bool sampleFdwRecheckForeignScan(ForeignScanState *node,
										TupleTableSlot *slot);
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
static Oid sampleFdwGetTableOid(List *table_options);


void
_PG_init(void)
{
	planner_hook_prev =	planner_hook;
	planner_hook = sample_fdw_palnner;

	set_join_pathlist_hook_prev = set_join_pathlist_hook;
	set_join_pathlist_hook = sample_fdw_set_join_pathlist;
}

static PlannedStmt * 
sample_fdw_palnner(Query *parse, int cursorOptions, ParamListInfo boundParams)
{
	PlannedStmt *result;

	printf("*** %s ***\n", __func__);

	use_sample_fdw = false;

	if (planner_hook_prev)
		result = (*planner_hook_prev)(parse, cursorOptions, boundParams);
	else
		result = standard_planner(parse, cursorOptions, boundParams);

	if (use_sample_fdw)
	{
		/* @todo */
	}

	return result;
}

static void
sample_fdw_set_join_pathlist(PlannerInfo *root,
							 RelOptInfo *joinrel,
							 RelOptInfo *outerrel,
							 RelOptInfo *innerrel,
							 JoinType jointype,
							 JoinPathExtraData *extra)
{
	if (set_join_pathlist_hook_prev)
		(*set_join_pathlist_hook_prev)(root, joinrel, outerrel, innerrel, jointype, extra);

	if (use_sample_fdw)
	{
#if 0
		ListCell *lc;

		printf("*** %s : %d ***\n", __func__, list_length(joinrel->pathlist));

		foreach(lc, joinrel->pathlist)
		{
			Path *path = (Path *) lfirst(lc);

			printf("\tnode=%d %d, rows=%f, startup_cost=%f, total_cost=%f\n",
				   path->type, path->pathtype, path->rows, path->startup_cost, path->total_cost);
		}
		/* @todo */
#endif
	}
}

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

	/* Functions for remote-join planning */
	routine->GetForeignJoinPaths = sampleFdwGetForeignJoinPaths;

	/* Functions for updating foreign tables */
	routine->AddForeignUpdateTargets = sampleFdwAddForeignUpdateTargets;
	routine->PlanForeignModify = sampleFdwPlanForeignModify;
	routine->BeginForeignModify = sampleFdwBeginForeignModify;
	routine->ExecForeignInsert = sampleFdwExecForeignInsert;
	routine->ExecForeignUpdate = sampleFdwExecForeignUpdate;
	routine->ExecForeignDelete = sampleFdwExecForeignDelete;
	routine->EndForeignModify = sampleFdwEndForeignModify;
	routine->IsForeignRelUpdatable = sampleFdwIsForeignRelUpdatable;

	/* Functions for SELECT FOR UPDATE/SHARE row locking */
	routine->GetForeignRowMarkType = sampleFdwGetForeignRowMarkType;
	routine->RefetchForeignRow = sampleFdwRefetchForeignRow;
	routine->RecheckForeignScan = sampleFdwRecheckForeignScan;

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
	HeapTuple tuple;
	Form_pg_class reltuple;
	float4 numtuples;
	List *quals = NIL;
	ListCell *lc;

	fdw_rel_info = (SampleFdwRelationInfo *) palloc0(sizeof(SampleFdwRelationInfo));

	fdw_rel_info->table = GetForeignTable(foreigntableid);
	fdw_rel_info->server = GetForeignServer(fdw_rel_info->table->serverid);

	fdw_rel_info->fdw_startup_cost = DEFAULT_FDW_STARTUP_COST;
	fdw_rel_info->fdw_tuple_cost = DEFAULT_FDW_TUPLE_COST;

	sample_fdw_oid = fdw_rel_info->server->fdwid;

#if 0
	foreach(lc, fdw_rel_info->server->options)
	{

	}
#endif

	table_oid = sampleFdwGetTableOid(fdw_rel_info->table->options);

	if (table_oid == 0)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("fff")));

	tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(table_oid));
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "cache lookup failed for relation %d", table_oid);
	reltuple = (Form_pg_class) GETSTRUCT(tuple);
	numtuples = reltuple->reltuples;
	ReleaseSysCache(tuple);

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

	fdw_rel_info->rows = numtuples;
	fdw_rel_info->width = baserel->width;
	fdw_rel_info->startup_cost = fdw_rel_info->fdw_startup_cost;
	fdw_rel_info->total_cost = fdw_rel_info->fdw_startup_cost + fdw_rel_info->fdw_tuple_cost * fdw_rel_info->rows;
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

static void
sampleFdwGetForeignJoinPaths(PlannerInfo *root,
							 RelOptInfo *joinrel,
							 RelOptInfo *outerrel,
							 RelOptInfo *innerrel,
							 JoinType jointype,
							 JoinPathExtraData *extra)
{
	SampleFdwRelationInfo *fdw_rel_info;
	ListCell *lc;

	printf("*** %s : %d ***\n", __func__, list_length(joinrel->pathlist));

	foreach(lc, joinrel->pathlist)
	{
		Path *path = (Path *) lfirst(lc);

		printf("\tnode=%d %d, rows=%f, startup_cost=%f, total_cost=%f\n",
			   path->type, path->pathtype, path->rows, path->startup_cost, path->total_cost);
	}

#if 0
	if (joinrel->fdw_private)
		return;

	fdw_rel_info = (SampleFdwRelationInfo *) palloc0(sizeof(SampleFdwRelationInfo));

	joinrel->fdw_private = (void *) fdw_rel_info;
#endif
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

	/*
	 * This `tlist' parameter may be a scan-all-rows targetlist generated by
	 * build_physical_tlist(). I want the original targetlist needed by by the
	 * query.
	 */
	tlist = build_path_tlist_external(root, &best_path->path);

	use_sample_fdw = true;

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
	ForeignScan *fscan = (ForeignScan *) node->ss.ps.plan;
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

	inner_state->remote_tableoid = linitial_oid(fscan->fdw_private);

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

			if (remoteAttr->attisdropped)
				continue;
			
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
				scan_tts->tts_values[attrnum - 1] = remote_tts->tts_values[i - 1];
				scan_tts->tts_isnull[attrnum - 1] = remote_tts->tts_isnull[i - 1];
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

static RowMarkType
sampleFdwGetForeignRowMarkType(RangeTblEntry *rte,
							   LockClauseStrength strength)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("GetForeignRowMarkType callback does not implemented")));

	return ROW_MARK_EXCLUSIVE;
}

static HeapTuple
sampleFdwRefetchForeignRow(EState *estate,
						   ExecRowMark *erm,
						   Datum rowid,
						   bool *updated)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("RefetchForeignRow callback does not implemented")));

	return NULL;
}

static bool
sampleFdwRecheckForeignScan(ForeignScanState *node,
							TupleTableSlot *slot)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("RecheckForeignScan callback does not implemented")));

	return false;
}

static void
sampleFdwExplainForeignScan(ForeignScanState *node,
							ExplainState *es)
{
	ForeignScan *fsplan = (ForeignScan *) node->ss.ps.plan;
 	Oid tableoid;
	ForeignServer *server;

	server = GetForeignServer(fsplan->fs_server);

	ExplainPropertyText("FDW", "sample_fdw", es);
	ExplainPropertyText("Remote Server", server->servername, es);

	tableoid = linitial_oid(fsplan->fdw_private);

	ExplainPropertyText("Remote Table",
						quote_qualified_identifier(get_namespace_name(get_rel_namespace(tableoid)),
												   get_rel_name(tableoid)),
						es);
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

static Oid
sampleFdwGetTableOid(List *table_options)
{
	ListCell *lc;

	foreach(lc, table_options)
	{
		DefElem	   *def = (DefElem *) lfirst(lc);

		if (strcmp(def->defname, "table_name") == 0)
		{
			RangeVar   *relvar;

			relvar = makeRangeVarFromNameList(charToQualifiedNameList(defGetString(def)));
			return RangeVarGetRelid(relvar, AccessShareLock, false);
		}
	}

	return (Oid) 0;
}
