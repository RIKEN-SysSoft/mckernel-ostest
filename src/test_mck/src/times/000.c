/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct tms buf;
	clock_t result;

	result = times(&buf);
	tp_assert(result != (clock_t)-1, "times() error");
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
