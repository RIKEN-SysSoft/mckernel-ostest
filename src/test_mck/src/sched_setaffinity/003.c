/* 003.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result = 0;
	size_t size;
	int num_cpus;
	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	num_cpus = args->num_of_cpus;
	size = CPU_ALLOC_SIZE(num_cpus);
  
	// migrate own process to other cpu.
	result = sched_setaffinity(0, size, (cpu_set_t *)-1 /* invalid address */);
	printf("sched_setaffinity result:%d, errno:%d (expect error is \"EFAULT\"=%d)\n", result, errno, EFAULT);
	tp_assert(result == -1, "Why sched_setaffinity didn't failed!?");
	tp_assert(errno == EFAULT, "Unexpected error is occur.");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
