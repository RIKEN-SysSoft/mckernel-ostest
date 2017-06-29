/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h> 
#include "test_mck.h"
#include "testsuite.h"

struct mmap_dev_args {
	const char* path;
	unsigned long size;
};

static const char* devmap_do_devmap(
	const char* path,
	unsigned long size)
{
	char *data;
	int status;
	int fd;
	int i;
	int pages;

	int val;
	unsigned char buf[PAGE_SIZE];

	tp_assert(path != NULL, "invalid argument(-d).");
	tp_assert(0 < size, "invalid argument(-s).");

	/* open */
	fd = open(path, O_RDWR);
	printf("open(%s)=%d\n", path, fd);
	tp_assert(fd != -1, "file open error.");
	
	/* ioctl */
	status = ioctl(fd, TEST_MCK_MMAP_DEV2_BUF_RESET);
	printf("ioctl()=%d\n", status);
	tp_assert(status != -1, "ioctl error.");
	
	/* mmap */
	data = (char*)mmap((caddr_t)0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	printf("mmap()=%p\n", data);
	tp_assert(data != (char*)-1, "mmap error.");

	/* accsess */
	pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	for (i = 0; i < pages; i++) {
		char *ptr = data + PAGE_SIZE * i;
		*ptr = 'Z';
	}
	
	/* validate */
	val = 1;
	for (i = 0; i < pages; i++) {
		int cmp;
		char *ptr = data + PAGE_SIZE * i;
		memset(buf, val, PAGE_SIZE);
		val++;
		buf[0] = 'Z';
		cmp = memcmp(ptr, buf, PAGE_SIZE);
		tp_assert(cmp == 0, "compare error.");
	}

	/* munmap */
	status = munmap(data, size);
	printf("munmap()=%d\n", status);
	tp_assert(status != -1, "munmap error.");
	
	/* close */ 
	close(fd);
	
	return NULL;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct mmap_dev_args args;
	char* ch;
	int opt;

	/*
	  d: device file path
	  s: map size
	*/
	while ((opt = getopt(tc_argc, tc_argv, "d:s:")) != -1) {
		switch (opt) {
		case 'd':
			args.path = optarg;
			break;
		case 's':
			args.size = strtoul(optarg, &ch, 10);
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mmap_dev_args* args = (struct mmap_dev_args*)tc_arg;
	tp_assert(args != NULL, "internal error");

	return devmap_do_devmap(args->path, args->size);
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
