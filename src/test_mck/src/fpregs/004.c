/* 004.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "arch_test_fpregs.h"

#define TST_SIGNAL SIGUSR1

static void fpregs004_sighandl(int signal)
{
	int i;
	struct fpregs fregs_sig;

	// make dirty.
	for(i=0; i<64; i++) {
		fregs_sig.ddata[i] = 0xdadadadadadadada;
	}
	fpregs_write(&fregs_sig);
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fpregs fregs0;

	if(signal(TST_SIGNAL, fpregs004_sighandl) == SIG_ERR) {
		tp_assert(1 == 0, "Set signal handler faild.");
	}

	memcpy(&fregs0, FPREGS_TEST_DATA, sizeof(struct fpregs));

	printf("[parent] before raise signal.\n");
	fpregs_write(&fregs0);
	show_current_fpregs();

	raise(TST_SIGNAL);

	printf("[parent] after raise signal.\n");
	show_current_fpregs();

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
