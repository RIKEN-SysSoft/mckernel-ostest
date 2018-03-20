/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "test_mck.h"
#include "testsuite.h"

#define TEMP_BUF_SIZE 0x1000000UL

extern int sig_count;

static void getrusage000_sig_handle(int sig)
{
	sig_count = 1;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char *tmp_buf = NULL;

	/* sighandler flag clear */
	sig_count = 0;

	/* set sighandler */
	tp_assert(signal(SIGALRM, &getrusage000_sig_handle) != SIG_ERR, "signal(SIGALRM) failed");

	/* initial show rusage */
	tp_assert(!send_and_show_rusage(RUSAGE_SELF), "getrusage() failed.");

	/* allocation memory */
	printf("allocation memory %ld byte(%ld KiB)\n", TEMP_BUF_SIZE, TEMP_BUF_SIZE / 1024);
	tmp_buf = mmap(0, TEMP_BUF_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (!tmp_buf) {
		tp_assert(0, "mmap() failed.");
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

	/* after added show rusage */
	tp_assert(!send_and_show_rusage(RUSAGE_SELF), "getrusage() failed.");

	/* free memory */
	printf("free memory %ld byte(%ld KiB)\n", TEMP_BUF_SIZE, TEMP_BUF_SIZE / 1024);
	munmap(tmp_buf, TEMP_BUF_SIZE);

	/* after free memory show rusage */
	tp_assert(!send_and_show_rusage(RUSAGE_SELF), "getrusage() failed.");

	return "you need check rusage value";
}
