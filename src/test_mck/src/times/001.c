/* 001.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct tms *invalid_buf;
	clock_t result;

	invalid_buf = (void*)0xffffffff00001000; // set invalid address;

	result = times(invalid_buf);
	tp_assert(result == (clock_t)-1, "Unexpected result.");
	tp_assert(errno == EFAULT, "Unexpected errno.");
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
