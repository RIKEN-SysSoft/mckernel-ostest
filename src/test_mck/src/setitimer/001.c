/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#define DELEGATE_NUMBER 0

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result;
	struct itimerval new_value = {{0, 0}, {0, 0}};

	result = setitimer(ITIMER_REAL, &new_value, (struct itimerval *)-1);
	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EFAULT, "Unexpected errno.");

	result = setitimer(ITIMER_REAL, (struct itimerval *)-1, NULL);
	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EFAULT, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
