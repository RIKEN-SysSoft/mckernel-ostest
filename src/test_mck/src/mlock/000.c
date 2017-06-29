/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdlib.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

#define MLOCK_MALLOC_SIZE 0x1000

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret;
	void *addr;

	addr = malloc(MLOCK_MALLOC_SIZE);
	tp_assert(addr != NULL, "malloc failed.");

	ret = mlock(addr, MLOCK_MALLOC_SIZE);
	tp_assert(ret != -1, "mlock failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
