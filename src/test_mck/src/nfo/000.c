/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <string.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static char* buf[2];
static const int len = sizeof(buf)/sizeof(buf[0]);

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	memset(buf, -1, sizeof(buf));
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	const int prot = PROT_READ;
	const int flags = MAP_PRIVATE|MAP_ANONYMOUS;
	int i;

	for (i = 0; i < len; i++) {
		buf[i] = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
		tp_assert(buf[i] != (void*)-1UL, "out of memory.(mmap)");
		printf("buf[%d]=%p\n", i, buf[i]);
	}

	// non faulting load.
	if (memcmp(buf[0], buf[1], PAGE_SIZE)) {
	}
	if (memcmp(buf[1], buf[0], PAGE_SIZE)) {
	}
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	for (i = 0; i < len; i++) {
		if (buf[i] != (void*)-1UL) {
			munmap(buf[i], PAGE_SIZE);
			buf[i] = (void*)-1UL;
		}
	}
}
