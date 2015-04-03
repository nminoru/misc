#include "postgres.h"
#include <string.h>
#include <signal.h>
#include <time.h>
#include "executor/executor.h"
#include "fmgr.h"
#include "nodes/nodes.h"
#include "tcop/tcopprot.h"
#include "utils/elog.h"

PG_MODULE_MAGIC;

extern void _PG_init(void);

static ExecutorRun_hook_type    executor_run_prev;
static timer_t timerid;
static struct itimerspec interval;
static struct sigevent env;

static void executor_run_routine(QueryDesc *queryDesc, ScanDirection direction, long count);
static void plan_profiler_signal_handler(SIGNAL_ARGS);

static volatile unsigned int sampling_count;
static unsigned int histogram_data[T_LimitState - T_PlanState + 1];
static void print_statics(void);

void
_PG_init(void)
{
	executor_run_prev   = ExecutorRun_hook;
    ExecutorRun_hook    = executor_run_routine;

	pqsignal(SIGRTMIN, plan_profiler_signal_handler);

	memset(&env,      0, sizeof(env));
	memset(&interval, 0, sizeof(interval));

	env.sigev_notify = SIGEV_SIGNAL;
	env.sigev_signo = SIGRTMIN;

	interval.it_interval.tv_sec  = 0;
	interval.it_interval.tv_nsec = 10 * 1000; /* 10 msec */
	interval.it_value.tv_sec     = 0;
	interval.it_value.tv_nsec    = 10 * 1000;
}

static void
executor_run_routine(QueryDesc *queryDesc, ScanDirection direction, long count)
{
	int ret;

	sampling_count = 0;

	memset(&histogram_data, 0, sizeof(histogram_data));

	ret = timer_create(CLOCK_REALTIME, &env, &timerid);
	if (ret != 0)
		elog(ERROR, "timer_create() errno=%d", ret);

	ret = timer_settime(timerid, 0, &interval, NULL);
	if (ret != 0)
		elog(ERROR, "timer_settime() errno=%d", ret);

	PG_TRY();
	{
		if (executor_run_prev)
			executor_run_prev(queryDesc, direction, count);
		else
			standard_ExecutorRun(queryDesc, direction, count);
	}
	PG_CATCH();
	{
		ret = timer_delete(timerid);
		if (ret != 0)
			elog(ERROR, "timer_delete() errno=%d", ret);

		PG_RE_THROW();
	}
	PG_END_TRY();

	ret = timer_delete(timerid);
	if (ret != 0)
		elog(ERROR, "timer_delete() errno=%d", ret);

	if (sampling_count >= 100)
		print_statics();
}

static void
print_statics(void)
{
	NodeTag t;

	printf("sampling_count = %d\n", sampling_count);
	printf("STATEMENT: %s\n", debug_query_string);
	
	for (t = T_PlanState ; t <= T_LimitState ; t++)
	{
		if (histogram_data[t - T_PlanState] > 0)
		{
			printf("%3u: %3.2f%%\n", t, 100.0 * histogram_data[t - T_PlanState] / sampling_count);
		}
	}

	printf("\n");
}

static void
plan_profiler_signal_handler(SIGNAL_ARGS)
{
	NodeTag tag = CurrentExecutingPlanState;

	if ((T_PlanState <= tag) && (tag <= T_LimitState))
	{
		sampling_count++;
		histogram_data[tag - T_PlanState]++;
	}
}
