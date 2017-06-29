/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <errno.h>

#include <string.h>
#include "arch_test_fpregs.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fpregs fregs0;
	pid_t pid;
	int status = 1;

	memcpy(&fregs0, FPREGS_TEST_DATA, sizeof(struct fpregs));

	printf("[parent] before fork.\n");
	fpregs_write(&fregs0);
	show_current_fpregs();

	pid = fork();
	switch (pid) {
	case -1:
		/* error */
		printf("ERROR: fork()=%d, errno=%d\n", pid, errno);
		break;
	case 0:
		/* child process */
		printf("[child] after fork.\n");
		show_current_fpregs();
		_exit(0);
		break;
	default:
		/* parent process */
		waitpid(pid, &status, 0);
		// printf("[parent] after fork.\n");
		// show_current_fpregs();
		break;
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
