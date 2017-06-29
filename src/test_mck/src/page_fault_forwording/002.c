/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <unistd.h>
#include <string.h>
#include "test_mck.h"
#include "testsuite.h"

static char* addr = (void*)-1;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char str[] = "hello world";
	size_t increment = sizeof(str);
	
	/* alloc */
	addr = sbrk(increment);
	
	/* copy */
	strcpy(addr, str);

	/* print */
	printf("%s\n", addr);

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free */
	if (addr != (void*)-1) {
		brk(addr);
	}
}

