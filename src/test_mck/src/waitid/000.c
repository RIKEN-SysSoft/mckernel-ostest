/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
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
	siginfo_t info;

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
		result = waitid(P_PID, pid, &info, WEXITED);
		break;
	}

	printf("waitid()=%d, errno=%d\n", result, errno);
	tp_assert(result != -1, "waitid failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
