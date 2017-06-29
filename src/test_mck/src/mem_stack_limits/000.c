/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

#define ULONG_1KiB_COUNT 0x80
#define ULONG_1MiB_COUNT 0x20000
#define ULONG_1GiB_COUNT 0x8000000

int kib_count = 0;
int mib_count = 0;
int gib_count = 0;
unsigned long last_sp = 0;
unsigned long first_sp = 0;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct mem_stack_limits_args args;
	char *ch;
	int opt;

	memset(&args, 0, sizeof(args));

	/*
	  s: The number of bytes to stack.
	*/
	while ((opt = getopt(tc_argc, tc_argv, "s:")) != -1) {
		switch (opt) {
		case 's' :
			args.size =strtoull(optarg, &ch, 10);
			break;
		default :
			break;
		}
	}
	return &args;
}
static inline void stack_limit_func(void);
static void kib_func(void)
{
	unsigned long tmp[ULONG_1KiB_COUNT];

	for (tmp[1] = 0;  tmp[1] < sizeof(tmp); tmp[1] += PAGE_SIZE) {
		memset((char *)tmp + tmp[1], 0xFF, 1);
	}
	last_sp = get_stack_addr();

	kib_count--;
	stack_limit_func();
}

static void mib_func(void)
{
	unsigned long tmp[ULONG_1MiB_COUNT];

	for (tmp[1] = 0;  tmp[1] < sizeof(tmp); tmp[1] += PAGE_SIZE) {
		memset((char *)tmp + tmp[1], 0xFF, 1);
	}
	last_sp = get_stack_addr();

	mib_count--;
	stack_limit_func();
}

static void gib_func(void)
{
	unsigned long tmp[ULONG_1GiB_COUNT];

	for (tmp[1] = 0;  tmp[1] < sizeof(tmp); tmp[1] += PAGE_SIZE) {
		memset((char *)tmp + tmp[1], 0xFF, 1);
	}
	last_sp = get_stack_addr();

	gib_count--;
	stack_limit_func();
}

static inline void stack_limit_func(void)
{
	if (gib_count) {
		gib_func();
	} else if (mib_count) {
		mib_func();
	} else if (kib_count) {
		kib_func();
	}
}

static void value_to_unit(size_t value, int *gib, int *mib, int *kib)
{
	int i = 0;
	size_t tmp_size = value;

	*gib = tmp_size / 0x40000000;
	for (i = 0; i < *gib; i++) {
		tmp_size -= 0x40000000;
	}
	*mib = tmp_size / 0x100000;
	tmp_size -= *mib * 0x100000;

	*kib = tmp_size / 0x400;
	tmp_size -= *kib * 0x400;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mem_stack_limits_args* args = (struct mem_stack_limits_args*)tc_arg;
	size_t used_stack = 0;
	int used_gib = 0;
	int used_mib = 0;
	int used_kib = 0;

	first_sp = get_stack_addr();

	/* check param */
	tp_assert(args != NULL, "internal error.(args == NULL)");
	tp_assert(args->size != 0, "invalid parameter.(-s is zero)");

	value_to_unit(args->size, &gib_count, &mib_count, &kib_count);
	printf("call: %d GiB + %d MiB + %d KiB\n", gib_count, mib_count, kib_count);

	stack_limit_func();

	used_stack = first_sp - last_sp;
	value_to_unit(used_stack, &used_gib, &used_mib, &used_kib);
	printf("used_stack size: %ld (%d GiB + %d MiB + %d KiB)\n", used_stack, used_gib, used_mib, used_kib);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
