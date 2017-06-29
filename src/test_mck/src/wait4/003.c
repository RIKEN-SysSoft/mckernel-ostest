/* 003.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

static void sig_handler(int sig)
{
	printf("(%s)sig_num=%d, pid=%d\n", __func__, sig, getpid());
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* Set SIGCHLD handler */
	struct sigaction sa;

	sa.sa_sigaction = (void *)sig_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGUSR2, &sa, NULL);

	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = 0;
	pid_t pid;

	int status = 1;

	pid = fork();
	switch(pid){
	case -1:
		break;
	case 0:
		/* child process */
		printf("[child]pid=%d\n", getpid());
		sleep(1);
		if(kill(getppid(), SIGUSR2) == -1) {
			fprintf(stderr, "[child]send signal failed.\n");
			exit(1);
		} else {
			printf("[child]send signal success.\n");
			sleep(1);
			exit(0);
		}
		break;
	default:
		/* parent process */
		//printf("[parent]pid=%d\n", getpid());
		result = wait(&status);

		printf("wait()=%d, errno=%d\n", result, errno);
		tp_assert(result != -1, "wait failed.");
		tp_assert(WTERMSIG(status) == 0, "child exitied with signal");

		break;
	}


	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
