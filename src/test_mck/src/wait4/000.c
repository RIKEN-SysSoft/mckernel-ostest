/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

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

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
