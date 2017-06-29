/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <errno.h>

#include "test_mck.h"
#include "testsuite.h"

#define SIGSETSIZE (_NSIG / 8)
volatile int rt_sigprocmask000_sig_count = 0;

void rt_sigprocmask000_sig_handler(int sig)
{
	rt_sigprocmask000_sig_count++;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret;
	struct sigaction act, oact;

	memset(&act, 0, sizeof(act));
	memset(&oact, 0, sizeof(oact));

	act.sa_handler = rt_sigprocmask000_sig_handler;
	sigset_t set, oset;

	tp_assert(sigemptyset(&set) >= 0, "sigemptyset call failed");
	tp_assert(sigaddset(&set, SIGUSR1) >= 0, "sigaddset call failed");


	/* call rt_sigaction() */
	ret = sigaction(SIGUSR1, &act, &oact);
	tp_assert(ret >= 0, "rt_sigaction call failed");

	/* call rt_sigprocmask() to block signal#SIGUSR1 */
	ret = sigprocmask(SIG_BLOCK, &set, &oset);
	tp_assert(ret == 0, "rt_sigprocmask call failed");

	/* Make sure that the masked process is indeed masked. */
	tp_assert(kill(getpid(), SIGUSR1) >= 0, "call to kill() failed");
	tp_assert(rt_sigprocmask000_sig_count == 0, 
		"rt_sigprocmask() failed to change the process's signal mask");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
