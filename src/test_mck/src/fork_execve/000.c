/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

void child_execve(struct fork_execve_args* args)
{
	char **argv;
	extern char **environ;
	int i = 0;

	argv = malloc(args->argc * (sizeof(char **) + 2));
	argv[0] = args->file_path;
	for (i = 0; i < args->argc; i++) {
		argv[i + 1] = *(args->argv + i);
	}
	argv[i + 1] = NULL;

	execve(args->file_path, argv, environ);
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fork_execve_args* args;
	args = fork_execve_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fork_execve_args* args;
	pid_t pid;
	int result = 0;
	int status = 1;
	struct rusage rusage;

	args = tc_arg;
	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->file_path != NULL, "invalid file path.");

	pid = fork();
	switch (pid) {
	case -1:
		/* error */
		printf("ERROR: fork()=%d, errno=%d\n", pid, errno);
		break;
	case 0:
		/* child process */
		child_execve(args);
		tp_assert(0, "Why return from execve() ???");
		break;
	default:
		/* parent process */
		result = wait4(pid, &status, 0, &rusage);
		if (result == -1) {
			printf("ERROR: wait4()=%d, errno=%d\n", result, errno);
		}
		break;
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
