/* 003.c COPYRIGHT FUJITSU LIMITED 2015 */
/*
 * vec が無効なアドレスを指しているときはエラーとなること
 */
#include <unistd.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

#define LENGTH_VALUE (PAGE_SIZE * 2)

static void* addr = MAP_FAILED;
static size_t length = LENGTH_VALUE;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int prot = PROT_WRITE | PROT_READ;
	int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED | MAP_POPULATE;
	unsigned long vec = ~0UL;
	int ret;

	/* map */
	addr = mmap(NULL, length, prot, flags, -1, 0);
	tp_assert(addr != MAP_FAILED, "mmap error.");

	/* access */
	memset(addr, 0xFF, length);

	/* call */
	ret = mincore(addr, length, (void*)vec);
	tp_assert(ret == -1, "mincore error.");
	tp_assert(errno == EFAULT, "invalid errno.");

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (addr != MAP_FAILED) {
		munmap(addr, length);
	}
}
