/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#define DELEGATE_NUMBER 0

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct itimerval new_value = {{0, 0}, {0, 0}};
	int result;

	result = setitimer(-1, &new_value, NULL);
	tp_assert(result == -1, "Unexpected result.");
	tp_assert(errno == EINVAL, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
