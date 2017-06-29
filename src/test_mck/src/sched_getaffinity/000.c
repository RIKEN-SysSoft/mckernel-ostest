/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return sched_getaffinity_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_getaffinity_args *args = (struct sched_getaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;
	int current_cpuid = get_cpu_id();

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);

	// get own affinity
	result = sched_getaffinity(0, size, cpusetp);
	if(result == -1){
		CPU_FREE(cpusetp);
		tp_assert(0, "sched_getaffinity failed.");
	}

	result = CPU_ISSET(current_cpuid, cpusetp);
	CPU_FREE(cpusetp);

	tp_assert(result != 0, "Really process running there?");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
