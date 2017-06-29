/* 115.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <signal.h>

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mremap_mmap_anon_param params[] = {
		{.size = MREMAP_RANGE_SIZE, .nopage = 0, .prot = PROT_READ|PROT_WRITE, .shared = 1}, // old > new(move)
		{.size = MREMAP_RANGE_SIZE, .nopage = 0, .prot = PROT_NONE, .shared = 1},            // use   use
	};
	size_t pcount = sizeof(params)/sizeof(params[0]);
	size_t old_size = params[0].size;
	size_t new_size = old_size + params[1].size;
	char* remap;

	{//init
		size_t maps;
		maps = mremap_mmap_anon(params, pcount);
		tp_assert(maps == pcount, "mmap error.");
	}

	{//remap
		char* addr = params[0].addr;
		errno = 0;
		remap = mremap(addr, old_size, new_size, MREMAP_MAYMOVE);
		tp_assert(remap != MAP_FAILED, "mremap error.");
		tp_assert(errno == 0, "invalid errno.");
	}

	{//verify
		const char* msg;
		msg = mremap_mmap_anon_data_check(remap, old_size, '0');
		tp_assert(msg == NULL, msg);
	}

	{//signal
		const char* msg;
		char ch;
		msg = mremap_mmap_anon_setup_signal_handler(SIGSEGV);
		tp_assert(msg == NULL, msg);
		ch = *params[0].addr;
		UNUSED_VARIABLE(ch);
	}
	return "signal handling error";
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
