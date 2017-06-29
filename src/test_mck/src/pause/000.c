/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "test_mck.h"
#include "testsuite.h"

static void pause000_sig_handle(int sig_num)
{
	printf("(%s)sig_num=%d\n", __func__, sig_num);
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int status = 0;
	(void)signal(SIGALRM, pause000_sig_handle);
	alarm(1);
	status = pause();
	printf("pause()=%d, errno=%d\n", status, errno);

	// pause() always return -1.
	tp_assert(status == -1, "pause call failed");
	
	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
