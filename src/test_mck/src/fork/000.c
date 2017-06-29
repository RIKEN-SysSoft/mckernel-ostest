/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

#include "test_mck.h"
#include "testsuite.h"

volatile int primitive_lock = 0;

static void child_pid(void)
{
	pid_t pid = getpid();
	pid_t tid = syscall(__NR_gettid);

	while(primitive_lock) {
		cpu_pause();
	}
	primitive_lock = 1;
	printf("[child]pid=%d, tid=%d\n", pid, tid);
	primitive_lock = 0;
}

static void parent_pid(void)
{
	pid_t pid = getpid();
	pid_t tid = syscall(__NR_gettid);

	while(primitive_lock) {
		cpu_pause();
	}
	primitive_lock = 1;
	printf("[parent]pid=%d, tid=%d\n", pid, tid);
	primitive_lock = 0;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret;

	ret = fork();

	if(ret == -1){
		/* error */
		tp_assert(0, "fork() Failed." );
	} else if (ret == 0) {
		/* child */
		child_pid();
		exit(0);
	} else {
		/* parent */
		parent_pid();
	}

	/* テスト成功 */ 
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
