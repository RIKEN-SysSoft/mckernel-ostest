/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <errno.h>

#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret;
	sigset_t *sigset;

	/* set sigset to point to an invalid location */
	sigset = (sigset_t *) - 1;

	ret = sigpending(sigset);
	tp_assert(ret == -1, "sigpending() Failed, expected return value=-1." );
	tp_assert(errno == EFAULT, "sigpending() Failed with wrong errno\n");

	/* システムコール呼び出しから返ってくれば、テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
