/* 109.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mremap_mmap_anon_param params[] = {
		{.size = MREMAP_RANGE_SIZE, .nopage = 0, .prot = PROT_NONE, .shared = 1},              // use > use
		{.size = MREMAP_RANGE_SIZE*2, .nopage = 0, .prot = PROT_READ|PROT_WRITE, .shared = 1}, // old   new(shrink)
		{.size = MREMAP_RANGE_SIZE, .nopage = 0, .prot = PROT_NONE, .shared = 1},              // use   use
	};
	size_t pcount = sizeof(params)/sizeof(params[0]);
	size_t old_size = params[1].size;
	size_t new_size = PAGE_SIZE;
	char* remap;

	{//init
		size_t maps;
		maps = mremap_mmap_anon(params, pcount);
		tp_assert(maps == pcount, "mmap error.");
	}

	{//remap
		char* addr = params[1].addr;
		char* naddr = params[1].addr + PAGE_SIZE;
		errno = 0;
		remap = mremap(addr, old_size, new_size, MREMAP_MAYMOVE|MREMAP_FIXED, naddr);
		tp_assert(remap == MAP_FAILED, "mremap error.");
		tp_assert(errno == EINVAL, "invalid errno.");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
