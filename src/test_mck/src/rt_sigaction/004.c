/* 004.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
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

	printf("sig#%d is handled.\n", sig);
	/* OK */
}

#define DELEGATE_NUMBER 0
SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	tp_assert(set_rt_sigaction_hundler(rt_sigaction_hundler, SA_RESETHAND, 0) == 0, "call failed unexpectedly");
	printf("/-------- Signal handler will activate -------/\n");
	(void) kill(getpid(), SIGUSR1);	// signal handler will be handle.
	printf("/------ Process will terminate by signal -----/\n");
	(void) kill(getpid(), SIGUSR1);	// will be terminate.
	sigaction_result_string = "signal handler called again?";
	return sigaction_result_string;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)
