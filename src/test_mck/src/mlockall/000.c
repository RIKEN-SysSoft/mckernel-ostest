/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int res;
	int flags = MCL_CURRENT|MCL_FUTURE;
	res = mlockall(flags);
	tp_assert(res == 0, "mlockall error.");
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
