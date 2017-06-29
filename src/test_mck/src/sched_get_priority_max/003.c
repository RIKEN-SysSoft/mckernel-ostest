/* 003.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sched.h>

#define SCHED_RR_MAX_PRIORITY 0

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int priority = 0;

	priority = sched_get_priority_max(SCHED_BATCH);
	printf("sched_get_priority_max(SCHED_BATCH) = %d, errno = %d\n", priority, errno);
	tp_assert(priority == SCHED_RR_MAX_PRIORITY, "sched_get_priority_max() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
