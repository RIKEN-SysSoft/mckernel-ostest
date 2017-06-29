/* 003.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result;
	result = setpgid(getpid(), getpgid(getpid())+1);
	printf("setpgid result:%d, errno:%d (expect error is \"EPERM\"=%d)\n", result, errno, EPERM);
	tp_assert(result == -1, "Why setpgid didn't failed!?");
    tp_assert(errno == EPERM, "Unexpected error is occur.");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
