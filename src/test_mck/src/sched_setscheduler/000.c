/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sched.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	int status = 0;
	int policy = 0;
	struct sched_param set_param = { 80 };
	struct sched_param get_param = { 0 };

	status = sched_setscheduler(pid, SCHED_RR, &set_param);
	printf("sched_setscheduler(%d, SCHED_RR, %p) = %d, errno = %d\n", pid, &set_param, status, errno);
	tp_assert(status != -1, "sched_setscheduler() failed.");

	policy = sched_getscheduler(pid);
	printf("sched_getscheduler(%d) = %d, errno = %d\n", pid, policy, errno);
	tp_assert(policy == SCHED_RR, "sched_getscheduler() failed.");

	status = sched_getparam(pid, &get_param);
	printf("sched_getparam(%d, %p) = %d, errno = %d\n", pid, &get_param, status, errno);
	tp_assert(status != -1, "sched_getparam() failed.");
	tp_assert(set_param.sched_priority == get_param.sched_priority, "sched_setscheduler() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
