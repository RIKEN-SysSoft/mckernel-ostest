/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return sched_setaffinity_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;

	int cpuid_before_migreate, cpuid_after_migreate;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);

	cpuid_before_migreate = get_cpu_id();

	// get own affinity
	result = sched_getaffinity(0, size, cpusetp);
	if(result == -1){
		CPU_FREE(cpusetp);
		tp_assert(0, "sched_getaffinity failed.");
	}

	// rewrite own affinity
	result = sched_setaffinity(0, size, cpusetp);
	if(result == -1){
		CPU_FREE(cpusetp);
		tp_assert(0, "sched_setaffinity failed.");
	}

	cpuid_after_migreate = get_cpu_id();

	CPU_FREE(cpusetp);

	tp_assert(cpuid_before_migreate == cpuid_after_migreate, "Really process running there?");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
