/* 005.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <signal.h>

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

static void child_sig_hadl(int sig)
{
	_exit(0);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_getaffinity_args *args = (struct sched_getaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp_parent, *cpusetp_child;
	size_t size;
	int num_cpus;
	int child_pid;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	child_pid = fork();

	if(child_pid == -1){
		/* error */
		tp_assert(0, "fork() Failed." );
	} else if (child_pid == 0) {
		/* child process */
		struct sigaction act;
		memset(&act, 0, sizeof(act));
		act.sa_handler = child_sig_hadl;
		sigaction(SIGUSR1, &act, NULL);

		// Wait for send signal to exit.
		while(1) {
			cpu_pause();
		}
	} else {
		/* parent process */
		num_cpus = args->num_of_cpus;
		size = CPU_ALLOC_SIZE(num_cpus);

		// get parent process affinity
		cpusetp_parent = CPU_ALLOC(num_cpus);
		tp_assert(cpusetp_parent != NULL, "(parent process) alloc failed.");
		CPU_ZERO_S(size, cpusetp_parent);
		result = sched_getaffinity(0, size, cpusetp_parent);
		if(result == -1){
			CPU_FREE(cpusetp_parent);
			tp_assert(0, "(parent process) sched_getaffinity failed.");
		}

		// get child process affinity
		cpusetp_child = CPU_ALLOC(num_cpus);
		if(cpusetp_child == NULL){
			CPU_FREE(cpusetp_parent);
			tp_assert(0, "(child process) alloc failed.");
		}
		CPU_ZERO_S(size, cpusetp_child);
		result = sched_getaffinity(child_pid, size, cpusetp_child);
		if(result == -1){
			CPU_FREE(cpusetp_parent);
			CPU_FREE(cpusetp_child);
			tp_assert(0, "(child process) sched_getaffinity failed.");
		}

		// check affinities are equal.
		result = CPU_EQUAL_S(size, cpusetp_parent, cpusetp_child);

		CPU_FREE(cpusetp_parent);
		CPU_FREE(cpusetp_child);

		// kill child process.
		kill(child_pid, SIGUSR1);

		tp_assert(result != 0, "child process is not taking over the parent cpu affinity.");
	}

	/* テスト成功 */ 
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
