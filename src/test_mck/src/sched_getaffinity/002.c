/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_getaffinity_args *args = (struct sched_getaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	int num_cpus;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	result = sched_getaffinity(0, 0 /* wrong size */, cpusetp);
	CPU_FREE(cpusetp);
	printf("sched_getaffinity result:%d, errno:%d (expect error is \"EINVAL\"=%d)\n", result, errno, EINVAL);
	tp_assert(result == -1, "Why sched_getaffinity didn't failed!?");
	tp_assert(errno == EINVAL, "Unexpected error is occur.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
