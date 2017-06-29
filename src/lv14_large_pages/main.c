/* main.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include "main.h"
#include <stdio.h>
#include <sys/mman.h>

enum test_anon_mode {
	TEST_ANON_NULL,
	TEST_ANON_READ,
	TEST_ANON_WRITE,
};

struct test_anon_arg
{
	/* mmap arg */
	size_t len;
	int prot;
	int flag;
	/* tp action */
	int repeat;
	enum test_anon_mode mode;
	size_t rw_off;
};

int test_anon(const struct test_anon_arg* args)
{
	char *addr = (void*)-1;
	int i = 0;
	char *print = NULL;

	for (i = 0; i < args->repeat; i++) {
		/* mmap */
		addr = mmap(NULL,
					args->len,
					args->prot,
					args->flag,
					-1,
					0);
		if (addr == (void*)-1) {
			perror("test_anon[mmap]:");
			return -1;
		}
		
		/* access */
		if (args->mode == TEST_ANON_READ)
		{
			print = addr + args->rw_off;
			printf("mmap-%d[%p-%p] %p=0x%lx\n",
				   i, addr, addr+args->len, print, *print);
		}
		else if (args->mode == TEST_ANON_WRITE)
		{
			print = addr + args->rw_off;
			*print = 'Z';
			printf("mmap-%d[%p-%p] %p=0x%lx\n",
				   i, addr, addr+args->len, print, *print);
		}
		else
		{
			munmap(addr, args->len);
			fprintf(stderr, "test_anon[]: invalid mode.");
			return -1;
		}
		
		/* munmap */
		if (munmap(addr, args->len) == -1) {
			perror("test_anon[munmap]:");
			return -1;
		}
	}
	return 0;
}


/*
 * - anonymous mapping
 * - 4MB
 * - ld
 */
int test00(void)
{
	struct test_anon_arg args;
	args.len =  _4MB;
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 1;
	args.mode = TEST_ANON_READ;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}

/*
 * - anonymous mapping
 * - 4MB
 * - st
 */
int test01(void)
{
	struct test_anon_arg args;
	args.len =  _4MB;
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 1;
	args.mode = TEST_ANON_WRITE;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}


/*
 * - anonymous mapping
 * - 256MB
 * - ld
 */
int test02(void)
{
	struct test_anon_arg args;
	size_t aligned_base = ALIGN_UP(TASK_UNMAPPED_BASE, _256MB);

	args.len =  _256MB + (aligned_base - TASK_UNMAPPED_BASE);
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 1;
	args.mode = TEST_ANON_READ;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}

/*
 * - type:   anonymous mapping
 * - size:   1GB
 * - ins:    ld
 * - repeat: 1
 */
int test03(void)
{
	struct test_anon_arg args;
	size_t aligned_base = ALIGN_UP(TASK_UNMAPPED_BASE, _1GB);

	args.len =  _1GB + (aligned_base - TASK_UNMAPPED_BASE);
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 1;
	args.mode = TEST_ANON_READ;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}

/*
 * - type:   anonymous mapping
 * - size:   1GB
 * - ins:    ld
 * - repeat: 2
 */
int test04(void)
{
	struct test_anon_arg args;
	size_t aligned_base = ALIGN_UP(TASK_UNMAPPED_BASE, _1GB);

	args.len =  _1GB + (aligned_base - TASK_UNMAPPED_BASE);
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 2;
	args.mode = TEST_ANON_READ;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}

/*
 * - type:   anonymous mapping
 * - size:   1GB
 * - ins:    ld
 * - repeat: 5
 */
int test05(void)
{
	struct test_anon_arg args;
	size_t aligned_base = ALIGN_UP(TASK_UNMAPPED_BASE, _1GB);

	args.len =  _1GB + (aligned_base - TASK_UNMAPPED_BASE);
	args.prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	args.flag = MAP_PRIVATE | MAP_ANONYMOUS;
	args.repeat = 5;
	args.mode = TEST_ANON_READ;
	args.rw_off = args.len - 8;

	return test_anon(&args);
}

#define ADD_TEST(name)					\
	{									\
		.run_msg =	"Run " #name ".",	\
		.fn = name,				        \
	}

int main(int argc, char** argv)
{
	
	struct {
		char* run_msg;
		int (*fn)(void);
	} tests[] = {
		ADD_TEST(test00),
		ADD_TEST(test01),
		ADD_TEST(test02),
		ADD_TEST(test03),
		ADD_TEST(test04),
		ADD_TEST(test05),
	};
	int i;
	int ret;

	if (argc < 2) {
		printf("%s <tp number>\n", argv[0]);
		return -1;
	}

	i = atoi(argv[1]);
	if (0 <= i && i < sizeof(tests)/sizeof(tests[0])) {
		printf("%s\n", tests[i].run_msg);
		ret = tests[i].fn();
		printf("%s\n", ret ? "error" : "success");
	} else {
		printf("invalid argument.\n");
		ret = -1;
	}
	return ret;
}

