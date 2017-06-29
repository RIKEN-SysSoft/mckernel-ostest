/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

static char* sigaction_result_string = NULL;

static void rt_sigaction_hundler(int sig, siginfo_t * sip, void *ucp)
{
	struct sigaction oact;
	int err;
	sigset_t nmask, omask;

	/* Get sigaction setting */
	err = sigaction(SIGUSR1, NULL, &oact);
	if (err == -1) {
		sigaction_result_string = "sigaction";
		return;
	}

	/* Get current signal mask */
	sigemptyset(&nmask);
	sigemptyset(&omask);
	err = sigprocmask(SIG_BLOCK, &nmask, &omask);
	if (err == -1) {
		sigaction_result_string = "sigprocmask";
		return;
	}

	/*
	 * In Linux, SA_RESETHAND doesn't imply SA_NODEFER; sig
	 * should not be masked.  The testcase should pass if
	 * SA_NODEFER is not masked, ie. if SA_NODEFER is a member
	 * of the signal list
	 */
	if (sigismember(&omask, sig) == 0) {
		sigaction_result_string = "SA_RESETHAND should cause sig to"
			 "be masked when the handler executes.";
		return;
	}
	/* OK */
}

#define DELEGATE_NUMBER 0
SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	tp_assert(set_rt_sigaction_hundler(rt_sigaction_hundler, SA_RESETHAND, 0) == 0, "call failed unexpectedly");
	(void) kill(getpid(), RT_SIGACTION_TEST_SIGNAL);
	return sigaction_result_string;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)
