#include "postgres.h"

#include "fmgr.h"
#include "nodes/nodes.h"
#include "optimizer/planner.h"
#include "utils/elog.h"

PG_MODULE_MAGIC;

extern void _PG_init(void);

static planner_hook_type previous_planner_hook;
static PlannedStmt *my_planner(Query *parse, int cursorOptions, ParamListInfo boundParams);

void
_PG_init(void)
{
	previous_planner_hook =	planner_hook;
	planner_hook = my_planner;
}

static PlannedStmt *
my_planner(Query *parse, int cursorOptions, ParamListInfo boundParams)
{
	char *message;

	message = nodeToString(parse);

	elog(LOG, "%s", message);
	
	if (previous_planner_hook)
		return (*previous_planner_hook)(parse, cursorOptions, boundParams);
	else
		return standard_planner(parse, cursorOptions, boundParams);
}
