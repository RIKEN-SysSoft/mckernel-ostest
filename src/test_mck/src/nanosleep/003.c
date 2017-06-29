/* 003.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int sleep_result;
	struct timespec *invalid_req;

	invalid_req = (void*)0xffffffff00001000; // set invalid address;

	sleep_result = nanosleep(invalid_req, NULL);
	tp_assert(sleep_result == -1, "Unexpected result.");
	tp_assert(errno == EFAULT, "Unexpected errno.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
