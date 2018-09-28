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
	 * The following check, which checks if Linux returns -1 when
	 * the size isn't multiple of 8 is obsolete so the expected
	 * return value is modified to zero.
	 */
	size = primitive_ceil(nr_cpus, sizeof(unsigned long));
	expect_error = /* ((int)size) % sizeof(unsigned long) ? -1 : */0;

	CPU_ZERO_S(size, cpusetp);
	CPU_SET_S(nr_cpus - 1, size, cpusetp);

	result = sched_setaffinity(0, size, cpusetp);
	CPU_FREE(cpusetp);

	tp_assert(result == expect_error, "sched_setaffinity returned unexpected value.");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
