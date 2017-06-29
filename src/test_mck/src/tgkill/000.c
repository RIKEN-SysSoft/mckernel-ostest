/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

#include "test_mck.h"
#include "testsuite.h"

volatile int sig_count = 0;

void tgkill000_sig_handle(int sig)
{
	sig_count = 1;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	pid_t tid = syscall(__NR_gettid);

	tp_assert(signal(SIGUSR1, &tgkill000_sig_handle) != SIG_ERR, "signal(SIGUSR1, ..) failed");

	tp_assert(syscall(__NR_tgkill, pid, tid, SIGUSR1) == 0, "tgkill failed");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
