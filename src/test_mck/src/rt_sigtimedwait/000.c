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
	sigset_t uset;
	siginfo_t uinfo;
	struct timespec timeout;

	syscall(__NR_rt_sigtimedwait, uset, uinfo, &timeout, 8);

	/* システムコール呼び出しから返ってくれば、テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
