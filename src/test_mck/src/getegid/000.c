/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getegid_args *args;
	args = getegid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getegid_args *args = tc_arg;

	gid_t get_egid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->egid != 0, "invalid egid.");

	get_egid = getegid();
	printf("getegid() = %d\n", get_egid);

	status = setregid(args->egid, args->egid);
	printf("setregid(%d, %d) = %d, errno = %d\n", args->egid, args->egid, status, errno);
	tp_assert(status != -1, "setregid() failed.");

	get_egid = getegid();
	printf("getegid() = %d\n", get_egid);
	tp_assert(args->egid == get_egid, "getegid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
