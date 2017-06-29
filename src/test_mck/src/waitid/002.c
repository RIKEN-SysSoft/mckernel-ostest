/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct waitid_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(tc_argc, tc_argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			args.proc_num = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	int i = 0;
	pid_t pid;
	siginfo_t info;
	struct waitid_args *args = (struct waitid_args*)tc_arg;

	tp_assert(args != NULL, "-p <child proc num>");
	tp_assert(0 < args->proc_num, "-p <child proc num> invalid argument.");

	for (i = 0; i < args->proc_num; i++) {
		pid = fork();
		switch(pid){
		case -1:
			break;
		case 0:
			/* child process */
			sleep(i + 1);
			printf("[child:%5d] sleep %dsec done.\n", getpid(), i + 1);
			_exit(0);
			break;
		default:
			/* parent process */
			break;
		}
	}

	/* parent process */
	pid = getpid();
	for (i = 0; i < args->proc_num; i++) {
		printf("[parent:%5d] waitid(P_ALL) called.\n", pid);
		result = waitid(P_ALL, 0, &info, WEXITED);
		printf("[parent:%5d] waitid(P_ALL) returned. EXTED pid = %d\n", pid, info.si_pid);

		printf("[parent:%5d] waitid()=%d, errno=%d\n", pid, result, errno);
		tp_assert(result != -1, "waitid failed.");
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
