/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char *cmd = "echo Hello World";
	int status = 0;
	int error = 0;

	/* print Hello World */
	status = system(cmd);

	if (WIFEXITED(status)) {
		printf("cmd exit successed by status %d\n", WEXITSTATUS(status));

		if (WEXITSTATUS(status)) {
			error = 1;
		}
	}

	if (WIFSIGNALED(status)) {
		error = 1;
		printf("cmd exited by signal %d\n", WTERMSIG(status));

		if (WCOREDUMP(status)) {
			printf("dumped corefile.\n");
		}
	}

	/* error check. */
	tp_assert(error == 0, "TP failed.");

	/* TP successed. */
	return NULL;
}
