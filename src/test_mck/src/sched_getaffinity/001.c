/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = -1;
	result = sched_getaffinity(0, sizeof(cpu_set_t), (cpu_set_t *)-1 /* invalid address */);
	printf("sched_getaffinity result:%d, errno:%d (expect error is \"EFAULT\"=%d)\n", result, errno, EFAULT);
	tp_assert(result == -1, "Why sched_getaffinity didn't failed!?");
	tp_assert(errno == EFAULT, "Unexpected error is occur.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
