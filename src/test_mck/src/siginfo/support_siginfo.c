/* support_siginfo.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include "test_mck.h"
#include "testsuite.h"

extern int delivered_signal;
static void siginfo_handler(int sig, siginfo_t *sip, void *ucp)
{
	printf("Catch signal #%d\n"
		"  siginfo->si_signo = %d\n"
		"  siginfo->si_errno = %d\n"
		"  siginfo->si_code  = 0x%x\n", 
		sig, sip->si_signo, sip->si_errno, sip->si_code);

	delivered_signal |= (1 << sig);
	//printf("delivered_signal=%d\n", ++delivered_signal);
}

int setup_siginfo_handler(void)
{
	int result = 0;
	int signum;

	struct sigaction sa;
	sa.sa_sigaction = siginfo_handler;
	sa.sa_flags = SA_RESETHAND | SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	for(signum=1; signum<ARCH_S64FX_SIGRTMIN; signum++){
		if(signum == SIGKILL || signum == SIGSTOP) {
			continue;
		} else {
			result = sigaction(signum, &sa, NULL);
		}

		/* There is something wrong? */
		if(result != 0) break;
	}

	return result;
}
