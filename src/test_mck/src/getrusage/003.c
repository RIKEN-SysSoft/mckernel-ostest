/* 003.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct rusage usage;
	int ret = 0;

	memset(&usage, 0, sizeof(struct rusage));

	/* invalid arg0 */
	ret = getrusage(0x12345, &usage);
	tp_assert(ret, "getrusage() why successed ?");

	if (errno != EINVAL) {
		printf("errno = %d, expected value = EINVAL(%d)\n", errno, EINVAL);
		tp_assert(0, "errno different from the expected value.");
	}

	/* TP successed. */
	return NULL;
}
