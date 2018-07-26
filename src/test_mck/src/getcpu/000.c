/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <unistd.h>
#include <sched.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	unsigned int cpu;
	unsigned int node;
	int stat;
	unsigned int exp_cpu = sched_getcpu();

	tp_assert(0 <= exp_cpu, "Why sched_getcpu failed?");

	stat = getcpu(&cpu, &node);
	tp_assert(stat == 0, "getcpu error.");
	printf("exp_cpu = %d, getcpu()cpu = %d\n", exp_cpu, cpu);
	tp_assert(cpu == exp_cpu, "cpu# error.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
