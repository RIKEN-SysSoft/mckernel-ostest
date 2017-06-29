/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getresuid_args *args;
	args = getresuid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getresuid_args *args = tc_arg;

	uid_t get_ruid = 0;
	uid_t get_euid = 0;
	uid_t get_suid = 0;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->ruid != 0, "invalid ruid.");
	tp_assert(args->euid != 0, "invalid euid.");
	tp_assert(args->suid != 0, "invalid suid.");

	status = getresuid(&get_ruid, &get_euid, &get_suid);
	printf("getresuid(%d, %d, %d) = %d, errno = %d\n", get_ruid, get_euid, get_suid, status, errno);
	tp_assert(status != -1, "getresuid() failed.");

	status = setresuid(args->ruid, args->euid, args->suid);
	printf("setresuid(%d, %d, %d) = %d, errno = %d\n", args->ruid, args->euid, args->suid, status, errno);
	tp_assert(status != -1, "setresuid() failed.");

	status = getresuid(&get_ruid, &get_euid, &get_suid);
	printf("getresuid(%d, %d, %d) = %d, errno = %d\n", get_ruid, get_euid, get_suid, status, errno);
	tp_assert(status != -1, "getresuid() failed.");
	tp_assert(args->ruid == get_ruid, "setresuid() failed.");
	tp_assert(args->euid == get_euid, "setresuid() failed.");
	tp_assert(args->suid == get_suid, "setresuid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
