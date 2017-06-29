/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sched.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	int status = 0;
	struct sched_param dummy_param = { 60 };
	struct sched_param set_param = { 90 };
	struct sched_param get_param = { 0 };

	status = sched_setscheduler(pid, SCHED_FIFO, &dummy_param);
	printf("sched_setscheduler(%d, SCHED_FIFO, %p) = %d, errno = %d\n", pid, &dummy_param, status, errno);
	tp_assert(status != -1, "sched_setscheduler() failed.");

	status = sched_setparam(pid, &set_param);
	printf("sched_setparam(%d, %p) = %d, errno = %d\n", pid, &set_param, status, errno);
	tp_assert(status != -1, "sched_setparam() failed.");

	status = sched_getparam(pid, &get_param);
	printf("sched_getparam(%d, %p) = %d, errno = %d\n", pid, &get_param, status, errno);
	tp_assert(status != -1, "sched_getparam() failed.");
	tp_assert(set_param.sched_priority == get_param.sched_priority, "sched_setparam() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
