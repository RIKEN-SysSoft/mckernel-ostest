/* 003.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/mman.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	const int size = PAGE_SIZE * 3;
	const int flags = MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE;
	const int old_prot = PROT_READ|PROT_EXEC;
	const int new_prot = PROT_WRITE|PROT_READ|PROT_EXEC;
	char *p;
	int ret;

	p = mmap(NULL, size, old_prot, flags, -1, 0);
	printf("mapped=%p\n", p);
	tp_assert(p != (void*)-1UL, "out of memory.(mmap)");

	ret = mprotect(p + PAGE_SIZE, PAGE_SIZE, new_prot);
	tp_assert(ret == 0, "mprotect error.");	

	munmap(p, size);
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
