/* 005.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#define BIG_NG_CPUID 1024

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;

	num_cpus = BIG_NG_CPUID;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);

	result = sched_setaffinity(0, size, cpusetp /* cleared mask */);

	CPU_FREE(cpusetp);
	printf("sched_setaffinity result:%d, errno:%d (expect error is \"EINVAL\"=%d)\n", result, errno, EINVAL);
	tp_assert(result == -1, "Why sched_setaffinity didn't failed!?");
	tp_assert(errno == EINVAL, "Unexpected error is occur.");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
