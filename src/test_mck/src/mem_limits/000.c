/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

long long MEM_LIMITS_COUNT_INFINITE = -1;

static enum mem_limits_function str2func(const char* str)
{
	const struct {
		const char *str;
		enum mem_limits_function fn;
	} tbl[] = {
		{.str="malloc", .fn=MEM_LIMITS_FN_MALLOC},
		{.str="mmap",   .fn=MEM_LIMITS_FN_MMAP},
		{.str="brk",    .fn=MEM_LIMITS_FN_BRK},
	};
	const int tbl_size = sizeof(tbl)/sizeof(tbl[0]);
	enum mem_limits_function fn = MEM_LIMITS_FN_NULL;
	int i;

	for (i = 0; i < tbl_size; i++) {
		if (strcmp(tbl[i].str, str) == 0) {
			fn = tbl[i].fn;
			break;
		}
	}
	return fn;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct mem_limits_args args;
	char* ch;
	int opt;

	memset(&args, 0, sizeof(args));
	args.count = MEM_LIMITS_COUNT_INFINITE;

	/*
	  f: function name.(malloc, mmap, brk)
	  s: The number of bytes to allocate.
	  c: The number of allocate count.(default:infinit)
	*/
	while ((opt = getopt(tc_argc, tc_argv, "f:s:c:S:")) != -1) {
		switch (opt) {
		case 'f':
			args.fn = str2func(optarg);
			break;
		case 's':
			args.size = strtoull(optarg, &ch, 10);
			break;
		case 'c':
			args.count = strtoll(optarg, &ch, 10);
			break;
		case 'S':
			if(strcmp(optarg, "mmap") == 0) {
				args.size = MMAP_AREA_SIZE;
			}
			break;
		default:
			break;
		}
	}
	return &args;
}

static char* do_malloc(size_t size, long long count)
{
	char* p;
	long long i;
	size_t off;

	for (i = 0; i < count || count == MEM_LIMITS_COUNT_INFINITE; i++) {
		p = malloc(size);
		printf("alloc#%lld: p=%p\n", i, p);
		tp_assert(p != NULL, "out of memory.(malloc)");

		for (off = 0; off < size; off += PAGE_SIZE) {
			*(p + off) = 'Z';
		}
	}
	return NULL;
}

static char* do_mmap(size_t size, long long count)
{
	const int prot = PROT_WRITE;
	const int flags = MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE;
	char* p;
	long long i;
	size_t off;

	for (i = 0; i < count || count == MEM_LIMITS_COUNT_INFINITE; i++) {
		p = mmap(NULL, size, prot, flags, -1, 0);
		printf("alloc#%lld: p=%p\n", i, p);
		tp_assert(p != (void*)-1UL, "out of memory.(mmap)");

		for (off = 0; off < size; off += PAGE_SIZE) {
			*(p + off) = 'Z';
		}
	}
	return NULL;
}

static char* do_brk(size_t size, long long count)
{
	void* old;
	void* new;
	char* p;
	long long i;
	size_t off;

	for (i = 0; i < count || count == MEM_LIMITS_COUNT_INFINITE; i++) {
		/* check current data segment.*/
		old = sbrk(0);
		printf("alloc#%lld: old=%p, ", i, old);
		tp_assert(old != (void*)-1UL, "sbrk error.");

		/* increment .*/
		p = sbrk(size);
		tp_assert(p != (void*)-1UL, "out of memory.(brk)");

		/* check new data segment.*/
		new = sbrk(0);
		printf("new=%p\n", new);
		tp_assert(new != (void*)-1UL, "sbrk error.");
		tp_assert(new == (char*)old + size, "sbrk error.(size)");

		for (off = 0; off < size; off += PAGE_SIZE) {
			*(p + off) = 'Z';
		}
	}
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mem_limits_args* args = (struct mem_limits_args*)tc_arg;

	/* check param */
	tp_assert(args != NULL, "internal error.");
	tp_assert(args->fn != MEM_LIMITS_FN_NULL, "invalid parameter(-f).");
	tp_assert(args->size != 0, "invalid paramter(-s).");
	tp_assert(0 <= args->count || args->count == MEM_LIMITS_COUNT_INFINITE, "invalid paramter(-c).");

	switch (args->fn) {
	case MEM_LIMITS_FN_MALLOC:
		return do_malloc(args->size, args->count);
	case MEM_LIMITS_FN_MMAP:
		return do_mmap(args->size, args->count);
	case MEM_LIMITS_FN_BRK:
		return do_brk(args->size, args->count);
	default:
		return "internal error.";
	}
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
