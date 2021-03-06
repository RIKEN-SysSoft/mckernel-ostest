/* 006.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#ifndef INT_MAX
#define INT_MAX (unsigned int)((int)-1)
#endif

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;
	pid_t invalid_pid;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	// search non exists process.
	for (invalid_pid = 2; invalid_pid < INT_MAX; invalid_pid++) {
		if (kill(invalid_pid, 0) == -1) {
			if (errno == ESRCH)
				break;
			else if (errno == EPERM)
				invalid_pid = INT_MAX - 1;
		}
	}

	num_cpus = args->num_of_cpus;
	cpusetp = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp != NULL, "alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp);
	CPU_SET_S(num_cpus - 1, size, cpusetp);

	result = sched_setaffinity(invalid_pid, size, cpusetp);
	CPU_FREE(cpusetp);

	printf("sched_setaffinity result:%d, errno:%d (expect error is \"ESRCH\"=%d)\n", result, errno, ESRCH);
	tp_assert(result == -1, "Why sched_setaffinity didn't failed!?");
	tp_assert(errno == ESRCH, "Unexpected error is occur.");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
