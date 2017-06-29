/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	getrlimit_tc tc = {
		RLIMIT_CPU,
		"RLIMIT_CPU",
		TEST_GETRLIMIT_FAILUER,
		ENOSYS,
		"ENOSYS"
	};

	int result = 0;
	struct rlimit rlim;

	tp_assert(0 <= tc_num, "invalid tp number.");

	result = getrlimit(tc.resource, &rlim);
	printf("getrlimit(%s)=%d\n", tc.res_str, result);
	if(result != 0){
		printf("errno=%d (McK may return %s(=%d))\n", errno, tc.errno_str, tc.expect_errno);
	}
	tp_assert(result == tc.expect_result, "getrlimit failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
