/* 007.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0);

extern int primitive_ceil(int x, int y);

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int nr_cpus;
	int expect_error;

	tp_assert(args != NULL, "mandatory parameter '-p <num_of_cpus>'");

	nr_cpus = args->num_of_cpus;
	tp_assert(nr_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	cpusetp = CPU_ALLOC(nr_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	/*
	 * size : While having a bit of only can express the number of cores, 
	 *       and is not a value in multiples of 8.
	 *
	 * if nr_cpus equals to 64, then sched_setaffinity will return success.
	 */
	size = primitive_ceil(nr_cpus, sizeof(unsigned long));
	expect_error = ((int)size) % sizeof(unsigned long) ? -1 : 0;

	CPU_ZERO_S(size, cpusetp);
	CPU_SET_S(nr_cpus - 1, size, cpusetp);

	result = sched_setaffinity(0, size, cpusetp);
	CPU_FREE(cpusetp);

	tp_assert(result == expect_error, "sched_setaffinity returns unexpected error.");
	if(result == 0) {
		printf("sched_setaffinity result:%d, errno:%d (expect error is \"EINVAL\"=%d)\n", result, errno, EINVAL);
		tp_assert(errno == EINVAL, "Unexpected error is occur.");
	}

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
