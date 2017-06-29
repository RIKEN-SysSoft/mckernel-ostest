/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result = getitimer(ITIMER_REAL, (struct itimerval *)-1);
	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EFAULT, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
