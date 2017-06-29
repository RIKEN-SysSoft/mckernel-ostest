/* 002.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>

#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

void *tgkill002_child_func(void *arg)
{
	pid_t *c_tid = (pid_t *)arg;

	/* send thread-id */
	*c_tid = syscall(__NR_gettid);

	printf("[child]pid=%d, tid=%d\n", getpid(), *c_tid);
	
	while(1) {
		cpu_pause();
	}
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pthread_t child_th;
	pid_t child_tid = 0;

	int status = pthread_create(&child_th, NULL, tgkill002_child_func, &child_tid);
	tp_assert(status == 0, "failed to create thread");

	printf("[parent]sleep a few sec...\n");
	sleep(5);
	printf("[parent]wake up\n");

	/* send signal to child thread. */
	tp_assert(syscall(__NR_tgkill, getpid(), child_tid, SIGUSR1) == 0, "tgkill failed");

	while(1) {
		cpu_pause();
	}

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

