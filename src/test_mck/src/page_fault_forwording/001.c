/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <sys/mman.h>
#include <string.h>
#include "test_mck.h"
#include "testsuite.h"

static char* addr = (void*)-1;
static int addr_size = 0;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char str[] = "hello world";

	/* mmap */
	addr_size = sizeof(str);
	addr = mmap(NULL, addr_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	tp_assert(addr != (void*)-1, "mmap error.");
		
	/* copy */
	strcpy(addr, str);

	/* print */
	printf("%s\n", addr);
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (addr != (void*)-1) {
		munmap(addr, addr_size);
	}
}
