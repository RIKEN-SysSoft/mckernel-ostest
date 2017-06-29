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

void sig_handler(int sig)
{
	printf("(%s)sig_num=%d\n", __func__, sig);
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret, ret2;
	sigset_t set, set1, set2;
	struct sigaction act, oact;

	if (sigemptyset(&set) < 0) {
		tp_assert(0, "sigemptyset failed." );
	}

	memset(&act, 0, sizeof(act));
	memset(&oact, 0, sizeof(oact));
	act.sa_handler = sig_handler;

	ret = sigaction(SIGALRM, &act, &oact);
	tp_assert(ret != -1, "rt_sigaction failed." );

	ret = sigprocmask(SIG_UNBLOCK, 0, &set1);
	tp_assert(ret != -1, "rt_sigprocmask1 failed." );

	alarm(5);

	ret = syscall(__NR_rt_sigsuspend, &set, SIGSETSIZE);
	printf("sys_rt_sigsuspend()=%d, errno=%d\n", ret, errno);

	alarm(0);

	// rt_sigsuspend always return -1.
	tp_assert(ret == -1, "rt_sigsuspend failed." );

	ret2 = sigprocmask(SIG_UNBLOCK, 0, &set2);
	tp_assert(ret2 != -1, "rt_sigprocmask2 failed." );
	tp_assert(set1.__val[0] == set2.__val[0], "rt_sigsuspend failed to preserve signal mask" );

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
