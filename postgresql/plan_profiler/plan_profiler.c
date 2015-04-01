#include "postgres.h"
#include <string.h>
#include <signal.h>
#include <time.h>
#include "executor/executor.h"
#include "fmgr.h"
#include "utils/elog.h"

PG_MODULE_MAGIC;

extern void _PG_init(void);

static ExecutorRun_hook_type    executor_run_prev;
static timer_t timerid;
static struct itimerspec interval;

static void executor_run_routine(QueryDesc *queryDesc, ScanDirection direction, long count);
static void plan_profiler_signal_handler(SIGNAL_ARGS);

void
_PG_init(void)
{
	struct sigevent env;

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
	interval.it_value.tv_nsec    = 10 * 1000; /* 10 msec */

	if (timer_create(CLOCK_REALTIME, &env, &timerid) != 0)
		elog(ERROR, "timer_create()");
}

static void
executor_run_routine(QueryDesc *queryDesc, ScanDirection direction, long count)
{
	if (timer_settime(timerid, 0, &interval, NULL) != 0)
		elog(ERROR, "timer_settime()");

	PG_TRY();
	{
		if (executor_run_prev)
			executor_run_prev(queryDesc, direction, count);
		else
			standard_ExecutorRun(queryDesc, direction, count);
	}
	PG_CATCH();
	{
		timer_settime(timerid, 0, NULL, NULL);

		PG_RE_THROW();
	}
	PG_END_TRY();

	timer_settime(timerid, 0, NULL, NULL);
}

static void
plan_profiler_signal_handler(SIGNAL_ARGS)
{
	timer_settime(timerid, 0, &interval, NULL);
}
