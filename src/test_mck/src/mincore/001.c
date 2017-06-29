/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
/*
 * ページがメモリー内にあるかどうかを判定できること
 * length はページサイズの倍数に切り上げられて情報が取得できること
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
	unsigned char vec[(LENGTH_VALUE + PAGE_SIZE - 1) / PAGE_SIZE] = {0};
	int ret;
	int i;

	/* map */
	addr = mmap(NULL, length, prot, flags, -1, 0);
	tp_assert(addr != MAP_FAILED, "mmap error.");

	/* access */
	memset(addr, 0xFF, length);

	/* call */
	ret = mincore(addr, length - PAGE_SIZE + 1, vec);
	tp_assert(ret == 0, "mincore error.");

	/* check */
	for (i = 0; i < sizeof(vec)/sizeof(vec[0]); i++) {
		/* McKであれば常に真のはず */
		tp_assert((vec[i] & 1) == 1, "invalid vector value.");
	}

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (addr != MAP_FAILED) {
		munmap(addr, length);
	}
}
