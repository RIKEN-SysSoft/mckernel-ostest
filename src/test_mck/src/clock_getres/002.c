/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct timespec ts = {0};
	int result = clock_getres(1000/*invalid clock_id*/, &ts);

	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EINVAL, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
