/* 004.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <sys/resource.h>
#include <errno.h>

#include "test_mck.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;

	/* invalid arg1 */
	ret = getrusage(RUSAGE_SELF, NULL);
	tp_assert(ret, "getrusage() why successed ?");

	if (errno != EFAULT) {
		printf("errno = %d, expected value = EFAULT(%d)\n", errno, EFAULT);
		tp_assert(0, "errno different from the expected value.");
	}

	/* TP successed. */
	return NULL;
}
