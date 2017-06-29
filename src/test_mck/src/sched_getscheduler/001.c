/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sched.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	int status = 0;
	int before_policy = 0;
	int after_policy = 0;
	struct sched_param set_param = { 80 };

	before_policy = sched_getscheduler(0);
	printf("sched_getscheduler(%d) = %d, errno = %d\n", 0, before_policy, errno);
	tp_assert(before_policy != -1, "sched_getscheduler() failed.");

	status = sched_setscheduler(pid, SCHED_RR, &set_param);
	printf("sched_setscheduler(%d, SCHED_RR, %p) = %d, errno = %d\n", pid, &set_param, status, errno);
	tp_assert(status != -1, "sched_setscheduler() failed.");

	after_policy = sched_getscheduler(0);
	printf("sched_getscheduler(%d) = %d, errno = %d\n", 0, after_policy, errno);
	tp_assert(before_policy != after_policy, "sched_getscheduler() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
