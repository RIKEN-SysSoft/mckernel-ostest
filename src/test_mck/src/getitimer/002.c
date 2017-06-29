/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct itimerval current = {
		{0xdeadbeef, 0xdeadbeef}, /* it_interval */
		{0xdeadbeef, 0xdeadbeef}  /* it_value */
	};
	int result = getitimer(-1, &current);

	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EINVAL, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
