/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "test_mck.h"
#include "testsuite.h"

#define TEMP_BUF_SIZE 0x1000000UL

extern int sig_count;

static void getrusage001_sig_handle(int sig)
{
	sig_count = 1;
}

void getrusage001_child(void)
{
	char *tmp_buf = NULL;

	/* sighandler flag clear */
	sig_count = 0;

	/* set sighandler */
	if(signal(SIGALRM, &getrusage001_sig_handle) == SIG_ERR) {
		printf("signal(SIGALRM) failed.\n");
		return;
	}

	/* allocation memory */
	printf("allocation memory %ld byte(%ld KiB)\n", TEMP_BUF_SIZE, TEMP_BUF_SIZE / 1024);
	tmp_buf = malloc(TEMP_BUF_SIZE);
	if (!tmp_buf) {
		printf("malloc() failed.\n");
		return;
	}
	memset(tmp_buf, 0xff, TEMP_BUF_SIZE);

	/* user time add */
	printf("alarm 2 seconds wait.\n");
	alarm(2);

	while (!sig_count) {
		cpu_pause();
	}

	/* kernel time add */
	printf("sleep 2 seconds wait.\n");
	sleep(2);

	/* free memory */
	printf("free memory %ld byte(%ld KiB)\n", TEMP_BUF_SIZE, TEMP_BUF_SIZE / 1024);
	free(tmp_buf);

	return;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int status = 0;
	pid_t pid = 0;


	/* initial show rusage */
	tp_assert(!send_and_show_rusage(RUSAGE_CHILDREN), "getrusage() failed.");

	/* send fork, create child process */
	pid = fork();

	switch (pid) {
	case -1:
		/* error */
		tp_assert(0, "fork() Failed." );
		break;
	case 0:
		/* child process */
		getrusage001_child();
		exit(0);
		break;
	default:
		/* parent process */
		break;
	}

	/* child process wait */
	wait(&status);

	/* after show rusage */
	tp_assert(!send_and_show_rusage(RUSAGE_CHILDREN), "getrusage() failed.");

	return "you need check rusage value";
}
