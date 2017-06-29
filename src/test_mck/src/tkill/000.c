/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/stat.h>

#include "test_mck.h"

extern int sig_count;

void tkill000_sig_handle(int sig)
{
	sig_count = 1;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t tid = syscall(__NR_gettid);

	sig_count = 0;

	tp_assert(signal(SIGUSR1, &tkill000_sig_handle) != SIG_ERR, "signal(SIGUSR1, ..) failed");

	tp_assert(syscall(__NR_tkill, tid, SIGUSR1) == 0, "tkill failed");

	while (!sig_count) {
		cpu_pause();
	}
	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
