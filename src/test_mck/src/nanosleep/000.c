/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	time_t time_result;
	int sleep_result;

	double elapsed;
	time_t before_sleep, after_sleep;
	struct timespec req = {.tv_sec = 3, .tv_nsec = 5000};

	time_result = time(&before_sleep);
	// printf("before sleep: %s\n", ctime(&before_sleep));
	tp_assert(time_result != -1, "time error.");

	sleep_result = nanosleep(&req, NULL);
	tp_assert(sleep_result == 0, "time error.");

	time_result = time(&after_sleep);
	// printf("after  sleep: %s\n", ctime(&after_sleep));
	tp_assert(time_result != -1, "time error.");

	elapsed = difftime(after_sleep, before_sleep);
	printf("It has passed for %8.01f sec.\n", elapsed);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
