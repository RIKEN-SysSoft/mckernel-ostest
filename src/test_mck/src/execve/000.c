/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"


SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct execve_args* args;
	args = execve_parse_args(tc_argc, tc_argv);
	return args;
}


RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct execve_args* args;

	args = tc_arg;
	tp_assert(args->file_path != NULL, "invalid file path.");

	char *argv[1] = {0}, *env[1] = {0};

	execve(args->file_path, argv, env);

	/* システムコール呼び出しから返ってくれば、テスト成功 */

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
