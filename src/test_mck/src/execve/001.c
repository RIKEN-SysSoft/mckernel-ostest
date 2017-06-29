/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <libgen.h>
#include <stdlib.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct execve_args* args;
	args = execve_parse_args(tc_argc, tc_argv);
	return args;
}


RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct execve_args* args;

	char **argv;
	extern char **environ;
	int i = 0;

	args = tc_arg;
	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->file_path != NULL, "invalid file path.");

	argv = malloc(args->argc * (sizeof(char **) + 2));
	argv[0] = args->file_path;
	for (i = 0; i < args->argc; i++) {
		argv[i + 1] = *(args->argv + i);
	}
	argv[i + 1] = NULL;

	execve(args->file_path, argv, environ);

	/* execveは返らない */
	tp_assert(0==1, "Why return from execve() ???");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
