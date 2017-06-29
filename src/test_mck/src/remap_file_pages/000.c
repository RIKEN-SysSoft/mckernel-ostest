/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include "test_mck.h"
#include "testsuite.h"

static const char test_file_name[] = "remap_file_pages_tp-XXXXXX";
static char path[256];
static int fd = -1;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct remap_file_pages_args args;
	char* ch;
	int opt;

	memset(&args, 0, sizeof(args));
	args.dir = ".";

	/*
	  d: working directry path.
	  s: test file size.
	*/
	while ((opt = getopt(tc_argc, tc_argv, "d:s:")) != -1) {
		switch (opt) {
		case 'd':
			args.dir = optarg;
			break;
		case 's':
			args.file_size = strtoull(optarg, &ch, 10);
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	const char* err;
	const struct remap_file_pages_args* args = (struct remap_file_pages_args*)tc_arg;
	char *addr;
	int mmap_page;
	int file_page;
	
	/* create test file */
	snprintf(path, sizeof(path), "%s/%s", args->dir, test_file_name);
	err = remap_file_pages_create_file(path, args->file_size);
	printf("test file path=%s\n", path);
	if (err) {
		return err;
	}

	/* file open */
	fd = open(path, O_RDWR);
	tp_assert(fd != -1, "file open error.");

	/* file mmap */
	addr = mmap(0, args->file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	tp_assert(addr != (char*)-1, "mmap error.");
	printf("mmap[%p-%p)\n", addr, addr+args->file_size);

	/* remap */
	mmap_page = (args->file_size / PAGE_SIZE) - 1;
	file_page = 0;
	while (mmap_page >= 0) {
		char* remap_addr;
		int status;
		remap_addr = remap_file_pages_pgoff_to_addr(addr, mmap_page);
		status = remap_file_pages(remap_addr, PAGE_SIZE, 0, file_page, 0);
		tp_assert(status == 0, "remap error");
		mmap_page--;
		file_page++;
	}

	/* compare */
	err = remap_file_pages_compare(addr, args->file_size);
	if (err) {
		return err;
	}
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (fd != -1) {
		close(fd);
		printf("delete test file=%s\n", path);
		unlink(path);
	}
}
