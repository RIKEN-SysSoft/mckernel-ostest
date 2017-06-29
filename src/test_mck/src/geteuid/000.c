/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct geteuid_args *args;
	args = geteuid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct geteuid_args *args = tc_arg;

	uid_t get_euid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->euid != 0, "invalid euid.");

	get_euid = geteuid();
	printf("geteuid() = %d\n", get_euid);

	status = setreuid(args->euid, args->euid);
	printf("setreuid(%d, %d) = %d, errno = %d\n", args->euid, args->euid, status, errno);
	tp_assert(status != -1, "setreuid() failed.");

	get_euid = geteuid();
	printf("geteuid() = %d\n", get_euid);
	tp_assert(args->euid == get_euid, "geteuid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
