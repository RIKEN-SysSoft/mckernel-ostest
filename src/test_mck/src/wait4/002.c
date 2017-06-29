/* 002.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

extern int sig_count;
static void sig_handler(int sig)
{
	sig_count++;
	printf("(%s)sig_num=%d\n", __func__, sig);
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	sig_count = 0;
	/* Set SIGCHLD handler */
	struct sigaction sa;

	sa.sa_sigaction = (void *)sig_handler;
	sa.sa_flags = SA_RESETHAND | SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGCHLD, &sa, NULL);

	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	pid_t pid;

	int status = 1;
	struct rusage rusage;

	pid = fork();
	switch(pid){
	case -1:
		break;
	case 0:
		/* child process */
		sleep(1);
		_exit(0);
		break;
	default:
		/* parent process */
		result = wait4(pid, &status, 0, &rusage);
		break;
	}

	printf("wait4()=%d, errno=%d\n", result, errno);
	tp_assert(result != -1, "wait4 failed.");
	tp_assert(sig_count != 0, "Parent process has not received a SIGCHLD.");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
