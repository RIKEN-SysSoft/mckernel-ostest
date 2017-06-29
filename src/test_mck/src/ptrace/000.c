/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"

#include <unistd.h>
#include <errno.h>
#include <sys/ptrace.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long result=0;
	pid_t child_pid;

	child_pid = fork();

	switch(child_pid){
	case -1:
		tp_assert(0==1, "fork failed.");
		break;
	case 0:
		/* child */
		sleep(1);
		_exit(0);
		break;
	default:
		/* parent */
		break;
	}

	result = ptrace(PTRACE_KILL, child_pid, NULL, NULL);
	printf("ptrace()=%ld, errno=%d\n", result, errno);

	/* システムコール呼び出しから返ってくれば、テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
