/* rt_sigaction.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

int set_rt_sigaction_hundler(void *handler, int flags, int sig_to_mask)
{
	struct sigaction sa;

	sa.sa_sigaction = handler;
	sa.sa_flags = flags;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, sig_to_mask);

	return sigaction(RT_SIGACTION_TEST_SIGNAL, &sa, NULL);
}
