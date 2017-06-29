/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setregid_args *args;
	args = setregid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setregid_args *args = tc_arg;

	gid_t get_rgid;
	gid_t get_egid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->rgid != 0, "invalid rgid.");
	tp_assert(args->egid != 0, "invalid egid.");

	status = setregid(args->rgid, args->egid);
	printf("setregid(%d, %d) = %d, errno = %d\n", args->rgid, args->egid, status, errno);
	tp_assert(status != -1, "setregid() failed.");

	get_rgid = getgid();
	printf("getgid() = %d\n", get_rgid);
	tp_assert(args->rgid == get_rgid, "setregid() failed.");

	get_egid = getegid();
	printf("getegid() = %d\n", get_egid);
	tp_assert(args->egid == get_egid, "setregid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
