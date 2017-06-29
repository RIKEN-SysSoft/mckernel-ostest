/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <pthread.h>

#include "test_mck.h"
#include "testsuite.h"

#define TEMP_BUF_SIZE 0x1000000UL

extern int sig_count;

static void getrusage002_sig_handle(int sig)
{
	sig_count = 1;
}

void *getrusage002_child(void *arg)
{
	char *tmp_buf = NULL;

	sig_count = 0;

	/* child initial show rusage */
	printf("[child before]\n");
	if (send_and_show_rusage(RUSAGE_THREAD)) {
		return NULL;
	}

	/* set sighandler */
	if(signal(SIGALRM, &getrusage002_sig_handle) == SIG_ERR) {
		printf("signal(SIGALRM) failed.\n");
		return NULL;
	}

	/* allocation memory */
	printf("allocation memory %ld byte(%ld KiB)\n", TEMP_BUF_SIZE, TEMP_BUF_SIZE / 1024);
	tmp_buf = malloc(TEMP_BUF_SIZE);
	if (!tmp_buf) {
		printf("malloc() failed.\n");
		return NULL;
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

	/* after free memory show rusage */
	printf("[child after]\n");
	if (send_and_show_rusage(RUSAGE_THREAD)) {
		return NULL;
	}
	return NULL;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int status = 0;
	pthread_t child_th;

	/* initial show rusage */
	printf("[parent before]\n");
	tp_assert(!send_and_show_rusage(RUSAGE_THREAD), "getrusage() failed.");

	/* child thread create */
	status = pthread_create(&child_th, NULL, getrusage002_child, NULL);
	tp_assert(status == 0, "failed to create thread");

	/* wait child thread */
	pthread_join(child_th, NULL);

	/* after child thread done, show rusage */
	printf("[parent after]\n");
	tp_assert(!send_and_show_rusage(RUSAGE_THREAD), "getrusage() failed.");

	return "you need check rusage value";
}
