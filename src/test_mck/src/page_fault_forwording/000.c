/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char buf[] = "hello world";
	printf("%s\n", buf);
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
