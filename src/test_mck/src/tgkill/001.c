/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

#include "test_mck.h"
#include "testsuite.h"

extern int sig_count;

static void tgkill001_sig_handle(int sig)
{
	sig_count = 1;
}

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	tp_assert(signal(SIGUSR1, &tgkill001_sig_handle) != SIG_ERR, "signal(SIGUSR1, ..) failed");

	/* Send signal using by raise(3) */
	tp_assert(raise(SIGUSR1) == 0, "tgkill failed");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
