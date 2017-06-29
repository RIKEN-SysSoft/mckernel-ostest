/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
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
		 * SA_RESETHAND and SA_SIGINFO were set. SA_SIGINFO should
		 * be clear in Linux. In Linux kernel, SA_SIGINFO is not
		 * cleared in psig().
		 */
	if(!(oact.sa_flags & SA_SIGINFO)) {
		sigaction_result_string = "SA_RESETHAND should not "
			 "cause SA_SIGINFO to be cleared, but it was.";
		return;
	}
	if(sip == NULL){
		sigaction_result_string = "siginfo should not be NULL";
		return;
	}
	/* OK */
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	tp_assert(set_rt_sigaction_hundler(rt_sigaction_hundler, SA_RESETHAND | SA_SIGINFO, 0) == 0, "call failed unexpectedly");
	(void) kill(getpid(), RT_SIGACTION_TEST_SIGNAL);
	return sigaction_result_string;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
