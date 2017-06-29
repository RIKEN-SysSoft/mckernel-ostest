/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/fsuid.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setfsuid_args *args;
	args = setfsuid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setfsuid_args *args = tc_arg;

	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->fsuid != 0, "invalid uid.");

	status = setfsuid(args->fsuid);
	printf("setfsuid(%d) = %d, errno = %d\n", args->fsuid, status, errno);
	tp_assert(status != -1, "setfsuid() failed.");

	tp_assert(check_procfs_status_uid(args->fsuid) == 1, "setfsuid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
