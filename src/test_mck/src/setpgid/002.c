/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result;
	result = setpgid(-1 /* bad pid */, getpgid(getpid()));
	printf("setpgid result:%d, errno:%d (expect error is \"ESRCH\"=%d)\n", result, errno, ESRCH);
	tp_assert(result == -1, "Why setpgid didn't failed!?");
    tp_assert(errno == ESRCH, "Unexpected error is occur.");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
