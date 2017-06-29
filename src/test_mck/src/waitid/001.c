/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
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
	pid_t pgid;
	siginfo_t info;
	struct waitid_args *args = (struct waitid_args*)tc_arg;

	tp_assert(args != NULL, "-p <child proc num>");
	tp_assert(1 < args->proc_num, "-p <child proc num> invalid argument.");
	tp_assert(args->proc_num % 2 == 0, "-p <child proc num> invalid argument.");

	for (i = 0; i < args->proc_num; i++) {
		pid = fork();
		switch(pid){
		case -1:
			break;
		case 0:
			/* child process */
			pid = getpid();
			if (i < args->proc_num / 2) {
				result = setpgid(pid, pid);
				printf("[child:%5d] setpgid(%d, %d) = %d\n", pid, pid, pid, result);
			}
			pgid = getpgid(pid);
			sleep(i + 3);
			printf("[child:%5d, %5d] sleep %dsec done.\n", pid, pgid, i + 3);
			_exit(0);
			break;
		default:
			/* parent process */
			break;
		}
	}

	/* parent process */
	pid = getpid();
	pgid = getpgid(pid);
	for (i = 0; i < args->proc_num / 2; i++) {
		printf("[parent:%5d] waitid(P_PGID, %d) called.\n", pid, pgid);
		result = waitid(P_PGID, pgid, &info, WEXITED);
		printf("[parent:%5d] waitid(P_PGID, %d) returned. EXTED pid = %d\n", pid, pgid, info.si_pid);
		printf("[parent:%5d] waitid()=%d, errno=%d\n", pid, result, errno);
		tp_assert(result != -1, "waitid failed.");
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
