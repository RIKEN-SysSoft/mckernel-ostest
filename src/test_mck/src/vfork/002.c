/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

static void child_execve(struct vfork_args* args)
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
	struct vfork_args* args;
	args = vfork_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct vfork_args* args;
	pid_t pid;
	int result = 0;
	int status = 1;
	int error = 0;
	struct rusage rusage;

	args = tc_arg;
	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->file_path != NULL, "invalid file path.");

	pid = vfork();
	switch (pid) {
	case -1:
		/* error */
		printf("ERROR: vfork()=%d, errno=%d\n", pid, errno);
		error = 1;
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
			error = 1;
		}
		break;
	}
	tp_assert(error == 0, "TP failed.");
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
