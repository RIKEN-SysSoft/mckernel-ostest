/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

int delivered_signal;
SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	delivered_signal = 0;
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int signum;
	tp_assert(setup_siginfo_handler() == 0, "Setup signal handlers failed. What's happen?");

	for(signum = 1; signum < ARCH_S64FX_SIGRTMIN; signum++) {
		if(signum == SIGKILL || signum == SIGSTOP) continue;

		printf("=== raise signal #%d ===\n", signum);
		tp_assert(raise(signum) == 0, "raise signal failed");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
