/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setreuid_args *args;
	args = setreuid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setreuid_args *args = tc_arg;

	uid_t get_ruid;
	uid_t get_euid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->ruid != 0, "invalid ruid.");
	tp_assert(args->euid != 0, "invalid euid.");

	status = setreuid(args->ruid, args->euid);
	printf("setreuid(%d, %d) = %d, errno = %d\n", args->ruid, args->euid, status, errno);
	tp_assert(status != -1, "setreuid() failed.");

	get_ruid = getuid();
	printf("getuid() = %d\n", get_ruid);
	tp_assert(args->ruid == get_ruid, "setreuid() failed.");

	get_euid = geteuid();
	printf("geteuid() = %d\n", get_euid);
	tp_assert(args->euid == get_euid, "setreuid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
