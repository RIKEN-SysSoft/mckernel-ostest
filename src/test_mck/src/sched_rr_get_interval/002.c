/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sched.h>

#define MCK_RETURN_SEC 0
#define MCK_RETURN_NSEC 0

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	int status = 0;
	struct sched_param set_param = { 80 };
	struct timespec time = { -1, -1 };

	status = sched_setscheduler(pid, SCHED_FIFO, &set_param);
	printf("sched_setscheduler(%d, SCHED_FIFO, %p) = %d, errno = %d\n", pid, &set_param, status, errno);
	tp_assert(status != -1, "sched_setscheduler() failed.");

	status = sched_rr_get_interval(pid, &time);
	printf("sched_rr_get_interval(%d, %p) = %d, errno = %d\n", pid, &time, status, errno);
	printf("  time.tv_sec = %ld, time.tv_nsec = %ld\n", time.tv_sec, time.tv_nsec);
	tp_assert(status != -1, "sched_rr_get_interval() failed.");
	tp_assert(time.tv_sec == MCK_RETURN_SEC, "sched_rr_get_interval() failed.");
	tp_assert(time.tv_nsec == MCK_RETURN_NSEC, "sched_rr_get_interval() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
