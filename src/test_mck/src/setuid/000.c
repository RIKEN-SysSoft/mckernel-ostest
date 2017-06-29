/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setuid_args *args;
	args = setuid_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct setuid_args *args = tc_arg;

	uid_t get_uid;
	int status = 0;

	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->uid != 0, "invalid uid.");

	status = setuid(args->uid);
	printf("setuid(%d) = %d, errno = %d\n", args->uid, status, errno);
	tp_assert(status != -1, "setuid() failed.");

	get_uid = getuid();
	printf("getuid() = %d\n", get_uid);
	tp_assert(args->uid == get_uid, "setuid() failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
