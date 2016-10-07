/*-------------------------------------------------------------------------
 *
 * sample_fdw.h
 *		  Sample foreign-data wrapper
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#ifndef SAMPLE_FDW_H
#define SAMPLE_FDW_H

#include "access/heapam.h"
#include "executor/tuptable.h"
#include "foreign/foreign.h"
#include "nodes/nodes.h"
#include "nodes/relation.h"
#include "utils/relcache.h"



/* Default CPU cost to start up a foreign query. */
#define DEFAULT_FDW_STARTUP_COST	(1.0)

/* Default CPU cost to process 1 row (above and beyond cpu_tuple_cost). */
#define DEFAULT_FDW_TUPLE_COST		(0.01)

typedef struct SampleFdwRelationInfo
{
	Bitmapset	   *attrs_used;

	QualCost		qual_cost;
	Selectivity		qual_selectivity;

	double			rows;
	int				width;
	Cost			startup_cost;
	Cost			total_cost;

	Cost			fdw_startup_cost;
	Cost			fdw_tuple_cost;

	ForeignTable   *table;
	ForeignServer  *server;
	
	Oid				remote_tableoid;

} SampleFdwRelationInfo;


typedef struct SampleFdwScanState
{
	Relation		rel;
	int				nattrs;
	AttrNumber	   *attr_map;

	Oid				remote_tableoid;
	Relation		remote_relation;
	HeapScanDesc	remote_scandesc;
	TupleTableSlot *remote_tts;
	int				last_remote_var;

} SampleFdwScanState;

extern void sample_fdw_correct_path_cost_size(Path *path, Oid fwdOid);
extern bool path_tree_walker(Path *path, bool (*walker)(Path *path, void *context), void *context);

extern List *build_path_tlist_external(PlannerInfo *root, Path *path);


#endif   /* SAMPLE_FDW_H */
