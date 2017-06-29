/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
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
	int cpuid_before_migreate = get_cpu_id();

	float f0 = 3.14;
	float f1 = 42.195;
	float f2 = 53.85;
	float f3 = 909.909;

	unsigned long xg[] = { 0x00000000ffffffff, 0xffffffff00000000, 0x0000ffff0000ffff, 0xffff0000ffff0000 };
	float xf[] = { 11.11, 22.22, 33.33, 44.44 };

	tp_assert(2 <= args->num_of_cpus, "mandatory parameter '-p <num_of_cpus(2 or more)>'");

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);

	migrate_cpu = num_cpus - 1;
	if (migrate_cpu == cpuid_before_migreate) {
		migrate_cpu--;
	}
	CPU_SET_S(migrate_cpu, size, cpusetp);
  
	/*
	 * 各種レジスタに値を設定
	 */
	write_xg(xg[0], xg[1], xg[2], xg[3]);
	write_xf(&xf[0], &xf[1], &xf[2], &xf[3]);

	printf("==before migrate process==\n");
	printf("f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", f0, f1, f2, f3);
	show_extended_registers();

	// migrate own process to other cpu.
	result = sched_setaffinity(0, size, cpusetp);
	if(result == -1){
		CPU_FREE(cpusetp);
		tp_assert(0, "sched_setaffinity failed.");
	}

	CPU_FREE(cpusetp);

	/*
	 * 設定した値が復元されていることを確認
	 */
	printf("==after migrate process==\n");
	printf("f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", f0, f1, f2, f3);
	show_extended_registers();

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
