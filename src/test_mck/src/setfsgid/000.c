/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/fsuid.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setfsgid_args *args;
	args = setfsgid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setfsgid_args *args = tc_arg;

	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->fsgid != 0, "invalid gid.");

	status = setfsgid(args->fsgid);
	printf("setfsgid(%d) = %d, errno = %d\n", args->fsgid, status, errno);
	tp_assert(status != -1, "setfsgid() failed.");

	tp_assert(check_procfs_status_gid(args->fsgid) == 1, "setfsgid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
