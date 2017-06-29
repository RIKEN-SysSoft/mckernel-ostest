/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <signal.h>
#include <unistd.h>

#define TST_SIGNAL SIGINT

static void nanosleep_sig_handler(int signal)
{
	// nothing to do.
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid;
	int sleep_result;
	struct timespec req = {.tv_sec = 10, .tv_nsec = 5000};
	struct timespec rem = {.tv_sec = 0, .tv_nsec = 0};

	if(signal(TST_SIGNAL, nanosleep_sig_handler) == SIG_ERR) {
		tp_assert(1 == 0, "Set signal handler faild.");
	}

	pid = fork();
	switch (pid) {
		case -1:
			tp_assert(1 == 0, "fork faild.");
			break;
		case 0:
			// child process
			sleep(2);
			kill(getppid(), TST_SIGNAL);
			_exit(0);
			break;
		default:
			// parent process
			sleep_result = nanosleep(&req, &rem);
			tp_assert(sleep_result == -1, "Unexpected result.");
			tp_assert(errno == EINTR, "Unexpected errno.");
			printf("Remaining sleep time is %d.%09ld\n", (int)rem.tv_sec, rem.tv_nsec);
			break;
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
