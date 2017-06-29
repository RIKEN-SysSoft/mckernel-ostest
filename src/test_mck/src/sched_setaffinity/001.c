/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
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
	int migrate_cpu = 0;

	int cpuid_before_migreate, cpuid_after_migreate;

	tp_assert(2 <= args->num_of_cpus, "mandatory parameter '-p <num_of_cpus(2 or more)>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);

	cpuid_before_migreate = get_cpu_id();

	// migrate to max cpuid
	// already max cpuid, decrement
	migrate_cpu = num_cpus - 1;
	if (migrate_cpu == cpuid_before_migreate) {
		migrate_cpu--;
	}
	CPU_SET_S(migrate_cpu, size, cpusetp);

	// migrate own process to other cpu.
	result = sched_setaffinity(0, size, cpusetp);
	if(result == -1){
		CPU_FREE(cpusetp);
		tp_assert(0, "sched_setaffinity failed.");
	}

	cpuid_after_migreate = get_cpu_id();

	CPU_FREE(cpusetp);

	tp_assert(cpuid_before_migreate != cpuid_after_migreate, "Really process running there?");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
