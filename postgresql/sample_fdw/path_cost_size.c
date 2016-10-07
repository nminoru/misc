/*-------------------------------------------------------------------------
 * path_cost_size.c
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "miscadmin.h"
#include "nodes/nodes.h"
#include "nodes/relation.h"
#include "utils/elog.h"

#include "sample_fdw.h"

typedef struct
{
	int num_ftables_to_same_server;
	Oid first_serverid;
	bool cannot_push_down;
} search_ftable_context;

static bool find_sample_fdw_walker(Path *path, void *context);

void
sample_fdw_correct_path_cost_size(Path *path, Oid fwdOid)
{
	JoinPath *joinpath = NULL;
	search_ftable_context context;
	ForeignServer *server;

	switch (nodeTag(path))
	{
		case T_NestPath:
		case T_MergePath:
		case T_HashPath:
			joinpath = (JoinPath *) path;
			break;

		default:
			return;
	}

	context.num_ftables_to_same_server = 0;
	context.first_serverid = 0;
	context.cannot_push_down = false;

	path_tree_walker(joinpath->outerjoinpath, find_sample_fdw_walker, &context);

	if (context.cannot_push_down || context.num_ftables_to_same_server == 0)
		return;

	context.num_ftables_to_same_server = 0;

	path_tree_walker(joinpath->innerjoinpath, find_sample_fdw_walker, &context);

	if (context.cannot_push_down || context.num_ftables_to_same_server == 0)
		return;

	server = GetForeignServer(context.first_serverid);

	if (fwdOid == server->fdwid)
	{
	}
}


static bool
find_sample_fdw_walker(Path *path, void *context)
{
	search_ftable_context *search_context = (search_ftable_context *) context;

	if (IsA(path, ForeignPath))
	{
		RelOptInfo *rel = path->parent;

		if (search_context->first_serverid == 0)
		{
			search_context->first_serverid = rel->serverid;
			search_context->num_ftables_to_same_server= 1;
		}
		else if (search_context->first_serverid == rel->serverid)
			search_context->num_ftables_to_same_server++;
		else
			search_context->cannot_push_down = false;
	}

	return path_tree_walker(path, find_sample_fdw_walker, context);
}

bool
path_tree_walker(Path *path, bool (*walker)(Path *path, void *context), void *context)
{
	ListCell   *lc;

	if (path == NULL)
		return false;

	check_stack_depth();

	switch (nodeTag(path))
	{
		case T_IndexPath:
			break;

		case T_BitmapHeapPath:
			if (walker(((BitmapHeapPath *) path)->bitmapqual, context))
				return true;
			break;

		case T_BitmapAndPath:
			foreach(lc, (((BitmapAndPath *) path)->bitmapquals))
			{
				if (walker((Path *) lfirst(lc), context))
					return true;
			}
			break;

		case T_BitmapOrPath:
			foreach(lc, (((BitmapOrPath *) path)->bitmapquals))
			{
				if (walker((Path *) lfirst(lc), context))
					return true;
			}
			break;

		case T_NestPath:
		case T_MergePath:
		case T_HashPath:
			if (walker(((JoinPath *) path)->outerjoinpath, context))
				return true;
			if (walker(((JoinPath *) path)->innerjoinpath, context))
				return true;
			break;

		case T_TidPath:
			break;

		case T_ForeignPath:
			if (walker(((ForeignPath *) path)->fdw_outerpath, context))
				return true;
			break;

		case T_CustomPath:
			foreach(lc, (((CustomPath *) path)->custom_paths))
			{
				if (walker((Path *) lfirst(lc), context))
					return true;
			}
			break;

		case T_AppendPath:
			foreach(lc, (((AppendPath *) path)->subpaths))
			{
				if (walker((Path *) lfirst(lc), context))
					return true;
			}
			break;

		case T_MergeAppendPath:
			foreach(lc, (((MergeAppendPath *) path)->subpaths))
			{
				if (walker((Path *) lfirst(lc), context))
					return true;
			}
			break;

		case T_ResultPath:
			break;

		case T_MaterialPath:
			if (walker(((MaterialPath *) path)->subpath, context))
				return true;
			break;

		case T_UniquePath:
			if (walker(((UniquePath *) path)->subpath, context))
				return true;
			break;

		default:
			elog(ERROR, "could not dump unrecognized node type: %d",
				 (int) nodeTag(path));
			break;
	}

	return false;
}
