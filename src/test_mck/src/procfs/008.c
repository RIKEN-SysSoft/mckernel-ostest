/* 008.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define BUF_LEN 256

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	unsigned long *buf1 = NULL;
	unsigned long buf2;
	off_t offset = 0;
	off_t ret = 0;

	/* allocate */
	buf1 = calloc(PAGE_SIZE, 1);
	if (buf1 == NULL) {
		tp_assert(0, "calloc failed.");
	}
	printf("allocated: %#016lx\n", (off_t)buf1);
	*buf1 = 0xffffffffffffff;

	/* procfile name generate */
	snprintf(pfname, sizeof(pfname), "/proc/%d/pagemap", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* calc offset */
	offset = (off_t)buf1 * 8 / PAGE_SIZE;

	/* lseek */
	if ((ret = lseek(fd, offset, SEEK_SET)) == -1) {
		printf("lseek() failed. %d\n", errno);
		goto lseek_err;
	}

	/* read */
	if ((read(fd, &buf2, sizeof(buf2))) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s(offset:%#016lx):%#016lx\n", pfname, (off_t)offset, buf2);

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}
	free(buf1);
	tp_assert(0, "you need check McKernel Log & Dump PAGEMAP.");

/* error case */
lseek_err:
read_err:
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
	}

open_err:
close_err:
	free(buf1);
	tp_assert(0, "TP failed.");
}
