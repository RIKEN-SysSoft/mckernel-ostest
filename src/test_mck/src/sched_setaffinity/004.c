/* 004.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;
	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);
	CPU_SET_S(num_cpus - 1, size, cpusetp);
  
	result = sched_setaffinity(0, 0 /* wrong size */, cpusetp);
	CPU_FREE(cpusetp);
	printf("sched_setaffinity result:%d, errno:%d (expect error is \"EINVAL\"=%d)\n", result, errno, EINVAL);
	tp_assert(result == -1, "Why sched_setaffinity didn't failed!?");
	tp_assert(errno == EINVAL, "Unexpected error is occur.");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
