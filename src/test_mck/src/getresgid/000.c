/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getresgid_args *args;
	args = getresgid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getresgid_args *args = tc_arg;

	gid_t get_rgid = 0;
	gid_t get_egid = 0;
	gid_t get_sgid = 0;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->rgid != 0, "invalid rgid.");
	tp_assert(args->egid != 0, "invalid egid.");
	tp_assert(args->sgid != 0, "invalid sgid.");

	status = getresgid(&get_rgid, &get_egid, &get_sgid);
	printf("getresgid(%d, %d, %d) = %d, errno = %d\n", get_rgid, get_egid, get_sgid, status, errno);
	tp_assert(status != -1, "getresgid() failed.");

	status = setresgid(args->rgid, args->egid, args->sgid);
	printf("setresgid(%d, %d, %d) = %d, errno = %d\n", args->rgid, args->egid, args->sgid, status, errno);
	tp_assert(status != -1, "setresgid() failed.");

	status = getresgid(&get_rgid, &get_egid, &get_sgid);
	printf("getresgid(%d, %d, %d) = %d, errno = %d\n", get_rgid, get_egid, get_sgid, status, errno);
	tp_assert(status != -1, "getresgid() failed.");
	tp_assert(args->rgid == get_rgid, "setresgid() failed.");
	tp_assert(args->egid == get_egid, "setresgid() failed.");
	tp_assert(args->sgid == get_sgid, "setresgid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
