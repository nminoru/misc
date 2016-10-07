/*-------------------------------------------------------------------------
 *
 * createplan.c
 *	  Routines to create the desired plan for processing a query.
 *	  Planning is complete, we just need to convert the selected
 *	  Path into a Plan.
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/optimizer/plan/createplan.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <limits.h>
#include <math.h>

#include "access/stratnum.h"
#include "access/sysattr.h"
#include "catalog/pg_class.h"
#include "foreign/fdwapi.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "optimizer/clauses.h"
#include "optimizer/cost.h"
#include "optimizer/paths.h"
#include "optimizer/placeholder.h"
#include "optimizer/plancat.h"
#include "optimizer/planmain.h"
#include "optimizer/planner.h"
#include "optimizer/predtest.h"
#include "optimizer/prep.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/subselect.h"
#include "optimizer/tlist.h"
#include "optimizer/var.h"
#include "parser/parse_clause.h"
#include "parser/parsetree.h"
#include "utils/lsyscache.h"

#include "sample_fdw.h"

static List *build_path_tlist(PlannerInfo *root, Path *path);

static Node *replace_nestloop_params(PlannerInfo *root, Node *expr);
static Node *replace_nestloop_params_mutator(Node *node, PlannerInfo *root);


List *
build_path_tlist_external(PlannerInfo *root, Path *path)
{
	return build_path_tlist(root, path);
}

/*
 * Build a target list (ie, a list of TargetEntry) for the Path's output.
 */
static List *
build_path_tlist(PlannerInfo *root, Path *path)
{
	RelOptInfo *rel = path->parent;
	List	   *tlist = NIL;
	int			resno = 1;
	ListCell   *v;

	foreach(v, rel->reltargetlist)
	{
		/* Do we really need to copy here?	Not sure */
		Node	   *node = (Node *) copyObject(lfirst(v));

		/*
		 * If it's a parameterized path, there might be lateral references in
		 * the tlist, which need to be replaced with Params.  There's no need
		 * to remake the TargetEntry nodes, so apply this to each list item
		 * separately.
		 */
		if (path->param_info)
			node = replace_nestloop_params(root, node);

		tlist = lappend(tlist, makeTargetEntry((Expr *) node,
											   resno,
											   NULL,
											   false));
		resno++;
	}
	return tlist;
}


/*****************************************************************************
 *
 *	SUPPORTING ROUTINES
 *
 *****************************************************************************/

/*
 * replace_nestloop_params
 *	  Replace outer-relation Vars and PlaceHolderVars in the given expression
 *	  with nestloop Params
 *
 * All Vars and PlaceHolderVars belonging to the relation(s) identified by
 * root->curOuterRels are replaced by Params, and entries are added to
 * root->curOuterParams if not already present.
 */
static Node *
replace_nestloop_params(PlannerInfo *root, Node *expr)
{
	/* No setup needed for tree walk, so away we go */
	return replace_nestloop_params_mutator(expr, root);
}

static Node *
replace_nestloop_params_mutator(Node *node, PlannerInfo *root)
{
	if (node == NULL)
		return NULL;
	if (IsA(node, Var))
	{
		Var		   *var = (Var *) node;
		Param	   *param;
		NestLoopParam *nlp;
		ListCell   *lc;

		/* Upper-level Vars should be long gone at this point */
		Assert(var->varlevelsup == 0);
		/* If not to be replaced, we can just return the Var unmodified */
		if (!bms_is_member(var->varno, root->curOuterRels))
			return node;
		/* Create a Param representing the Var */
		param = assign_nestloop_param_var(root, var);
		/* Is this param already listed in root->curOuterParams? */
		foreach(lc, root->curOuterParams)
		{
			nlp = (NestLoopParam *) lfirst(lc);
			if (nlp->paramno == param->paramid)
			{
				Assert(equal(var, nlp->paramval));
				/* Present, so we can just return the Param */
				return (Node *) param;
			}
		}
		/* No, so add it */
		nlp = makeNode(NestLoopParam);
		nlp->paramno = param->paramid;
		nlp->paramval = var;
		root->curOuterParams = lappend(root->curOuterParams, nlp);
		/* And return the replacement Param */
		return (Node *) param;
	}
	if (IsA(node, PlaceHolderVar))
	{
		PlaceHolderVar *phv = (PlaceHolderVar *) node;
		Param	   *param;
		NestLoopParam *nlp;
		ListCell   *lc;

		/* Upper-level PlaceHolderVars should be long gone at this point */
		Assert(phv->phlevelsup == 0);

		/*
		 * Check whether we need to replace the PHV.  We use bms_overlap as a
		 * cheap/quick test to see if the PHV might be evaluated in the outer
		 * rels, and then grab its PlaceHolderInfo to tell for sure.
		 */
		if (!bms_overlap(phv->phrels, root->curOuterRels) ||
		  !bms_is_subset(find_placeholder_info(root, phv, false)->ph_eval_at,
						 root->curOuterRels))
		{
			/*
			 * We can't replace the whole PHV, but we might still need to
			 * replace Vars or PHVs within its expression, in case it ends up
			 * actually getting evaluated here.  (It might get evaluated in
			 * this plan node, or some child node; in the latter case we don't
			 * really need to process the expression here, but we haven't got
			 * enough info to tell if that's the case.)  Flat-copy the PHV
			 * node and then recurse on its expression.
			 *
			 * Note that after doing this, we might have different
			 * representations of the contents of the same PHV in different
			 * parts of the plan tree.  This is OK because equal() will just
			 * match on phid/phlevelsup, so setrefs.c will still recognize an
			 * upper-level reference to a lower-level copy of the same PHV.
			 */
			PlaceHolderVar *newphv = makeNode(PlaceHolderVar);

			memcpy(newphv, phv, sizeof(PlaceHolderVar));
			newphv->phexpr = (Expr *)
				replace_nestloop_params_mutator((Node *) phv->phexpr,
												root);
			return (Node *) newphv;
		}
		/* Create a Param representing the PlaceHolderVar */
		param = assign_nestloop_param_placeholdervar(root, phv);
		/* Is this param already listed in root->curOuterParams? */
		foreach(lc, root->curOuterParams)
		{
			nlp = (NestLoopParam *) lfirst(lc);
			if (nlp->paramno == param->paramid)
			{
				Assert(equal(phv, nlp->paramval));
				/* Present, so we can just return the Param */
				return (Node *) param;
			}
		}
		/* No, so add it */
		nlp = makeNode(NestLoopParam);
		nlp->paramno = param->paramid;
		nlp->paramval = (Var *) phv;
		root->curOuterParams = lappend(root->curOuterParams, nlp);
		/* And return the replacement Param */
		return (Node *) param;
	}
	return expression_tree_mutator(node,
								   replace_nestloop_params_mutator,
								   (void *) root);
}

