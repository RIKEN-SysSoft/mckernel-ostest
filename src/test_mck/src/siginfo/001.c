/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <sys/types.h>
#include <unistd.h>

#if defined(__sparc)
# define ARCH_SIGNAL_EXPECT_TO_DELIVER (1 << SIGEMT)
#elif defined(__x86)
# define ARCH_SIGNAL_EXPECT_TO_DELIVER 0
#else
# define ARCH_SIGNAL_EXPECT_TO_DELIVER 0
#endif

#define SIGNAL_EXPECT_TO_DELIVER ( \
	ARCH_SIGNAL_EXPECT_TO_DELIVER |	\
	(1 << SIGHUP) | \
	(1 << SIGINT) | \
	(1 << SIGQUIT) | \
	(1 << SIGILL) | \
	(1 << SIGTRAP) | \
	(1 << SIGABRT) | \
	(1 << SIGFPE) | \
	(1 << SIGBUS) | \
	(1 << SIGSEGV) | \
	(1 << SIGSYS) | \
	(1 << SIGPIPE) | \
	(1 << SIGALRM) | \
	(1 << SIGTERM) | \
	(1 << SIGURG) | \
	(1 << SIGIO) | \
	(1 << SIGXCPU) | \
	(1 << SIGXFSZ) | \
	(1 << SIGVTALRM) | \
	(1 << SIGPROF) | \
	(1 << SIGWINCH) | \
	(1 << SIGPWR) | \
	(1 << SIGUSR1) | \
	(1 << SIGUSR2))

extern int delivered_signal;

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	tp_assert(setup_siginfo_handler() == 0, "Setup signal handlers failed. What's happen?");

	printf("==================================================\n");
	printf("Please send signal to mcexec(pid=%d) from console.\n", getpid());
	printf("Exit Once you throw twice the same signal.\n");
	printf("==================================================\n");

	while(delivered_signal != SIGNAL_EXPECT_TO_DELIVER) {
		cpu_pause();
	}

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
