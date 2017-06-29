/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getgid_args *args;
	args = getgid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct getgid_args *args = tc_arg;

	gid_t get_gid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->gid != 0, "invalid gid.");

	get_gid = getgid();
	printf("getgid() = %d\n", get_gid);

	status = setgid(args->gid);
	printf("setgid(%d) = %d, errno = %d\n", args->gid, status, errno);
	tp_assert(status != -1, "setgid() failed.");

	get_gid = getgid();
	printf("getgid() = %d\n", get_gid);
	tp_assert(args->gid == get_gid, "getgid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
