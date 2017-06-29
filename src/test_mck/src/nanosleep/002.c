/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int sleep_result;
	struct timespec invalid_req[] = {
		{.tv_sec = -10, .tv_nsec = 5000},
		{.tv_sec = 10, .tv_nsec = 1000000000}
	};
	int i;

	for (i = 0; i < (sizeof(invalid_req)/sizeof(invalid_req[0])); ++i)
	{
		sleep_result = nanosleep(&invalid_req[i], NULL);
		tp_assert(sleep_result == -1, "Unexpected result.");
		tp_assert(errno == EINVAL, "Unexpected errno.");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
