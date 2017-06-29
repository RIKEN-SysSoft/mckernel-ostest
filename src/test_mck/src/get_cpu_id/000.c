/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int cpuid = get_cpu_id();

	tp_assert(cpuid != -1, "get wrong value.");

	printf("[pid=%d, tid=%d] process running on cpu=%d\n", getpid(), gettid(), cpuid);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
