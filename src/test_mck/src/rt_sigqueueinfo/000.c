/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/types.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	siginfo_t uinfo;

	uinfo.si_errno = 0;
	uinfo.si_code = SI_QUEUE;

	syscall(__NR_rt_sigqueueinfo, getpid(),SIGCHLD, &uinfo);

	/* システムコール呼び出しから返ってくれば、テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
