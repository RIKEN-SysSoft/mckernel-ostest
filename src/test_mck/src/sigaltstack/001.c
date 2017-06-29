/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "test_mck.h"
#include "testsuite.h"

void *sigaltstack1_sigstack_addr, *main_stk;		/* address of main stack for signal */
int got_signal = 0;

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

/*
 * void
 * sigaltstack1_sighandler() - signal catching function.
 *  This functions is called when the signal 'SIGUSR1' is delivered to
 *  the test process and trapped by sigaction().
 *
 *  This function updates 'sigaltstack1_sigstack_addr' variable and sets got_signal value.
 */
void sigaltstack1_sighandler(int n)
{
	int i;

	(void) n;
	sigaltstack1_sigstack_addr = &i;
	got_signal = 1;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int syscall_ret = 0;
	stack_t sigstk, osigstk;	/* signal stack storing struct. */
	struct sigaction act, oact;	/* sigaction() struct. */

	void *main_stk;		/* address of main stack for signal */
	void *alt_stk;		/* address of alternate stack for signal */

	pid_t my_pid = getpid();

	/**
	 * setup
	 */
	{
		/* Capture SIGUSR1 on the main stack */
		act.sa_handler = (void (*)(int))sigaltstack1_sighandler;
		syscall_ret = sigaction(SIGUSR1, &act, &oact);
		tp_assert(syscall_ret != -1, "sigaction() fails in setup");

		/* Send the signal to the test process */
		kill(my_pid, SIGUSR1);

		/* Wait till the signal arrives */
		while (!got_signal) {
			cpu_pause();
		}

		got_signal = 0;
		main_stk = sigaltstack1_sigstack_addr;	// save stack address.
	}

	/* Call sigaltstack() to set up an alternate stack */
	sigstk.ss_size = SIGSTKSZ;
	sigstk.ss_flags = 0;
	sigstk.ss_sp = (void *)malloc(SIGSTKSZ);
	tp_assert(sigstk.ss_sp != NULL, "could not allocate memory for the alternate stack");

	printf("alternate stack settings = {\n");
	printf("  ss_sp   = %p\n", sigstk.ss_sp);
	printf("  ss_size = 0x%x\n", SIGSTKSZ);
	printf("};\n");


	syscall_ret = sigaltstack(&sigstk, &osigstk);
	tp_assert(syscall_ret != -1, "sigaltstack() Failed");

	/* Set up the signal handler for 'SIGUSR1' */
	act.sa_flags = SA_ONSTACK;
	act.sa_handler = (void (*)(int))sigaltstack1_sighandler;
	syscall_ret = sigaction(SIGUSR1, &act, &oact);
	tp_assert(syscall_ret != -1, "sigaction() fails to trap signal delivered on alt. stack");

	/* Deliver signal onto the alternate stack */
	kill(my_pid, SIGUSR1);

	/* wait till the signal arrives */
	while (!got_signal) {
		cpu_pause();
	}

	got_signal = 0;
	alt_stk = sigaltstack1_sigstack_addr;

	printf("Actual altternalte stack = %p\n", alt_stk);
	printf("Actual main stack        = %p\n", main_stk);

	/*
	 * First, Check that alt_stk is within the alternate stk boundaries
	 * Second, Check that main_stk is outside the alternate stk boundaries.
	 */
	if ((alt_stk < sigstk.ss_sp) && (alt_stk > (sigstk.ss_sp + SIGSTKSZ))) {
		tp_assert(0==-1, "alt. stack is not within the alternate stk boundaries");
	} else if ((main_stk >= sigstk.ss_sp) && (main_stk <= (sigstk.ss_sp + SIGSTKSZ))) {
		tp_assert(0==-1, "main stk. not outside the alt. stack boundaries");
	}

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
