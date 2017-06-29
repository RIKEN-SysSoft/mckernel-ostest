/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
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
	int fd = 0, i = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	unsigned long *buf1 = NULL;
	unsigned long buf2[BUF_LEN / sizeof(unsigned long)];
	off_t dump_off = 0;
	off_t ret = 0;

	/* allocate */
	buf1 = calloc(BUF_LEN, 1);
	if (buf1 == NULL) {
		tp_assert(0, "calloc failed.");
	}
	printf("allocated: %#016lx\n", (off_t)buf1);

	/* filled magic number */
	for (i = 0; i < BUF_LEN / sizeof(unsigned long); i++) {
		buf1[i] = i;
	}

	/* procfile name generate */
	sprintf(pfname, "/proc/%d/mem", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* lseek */
	if ((ret = lseek(fd, (off_t)buf1, SEEK_SET)) == -1) {
		printf("lseek() failed. %d\n", errno);
		goto lseek_err;
	}

	/* read */
	if ((read(fd, buf2, BUF_LEN)) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s(offset:%#016lx):\n", pfname, (off_t)buf1);
	dump_off = (off_t)buf1;
	i = 0;
	while(i < BUF_LEN / sizeof(unsigned long)) {
		printf("  %#016lx: %016lx %016lx %016lx %016lx\n", dump_off, buf2[i], buf2[i + 1], buf2[i + 2], buf2[i + 3]);
		i += 4;
		dump_off += sizeof(unsigned long) * 4;
	}

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}
	free(buf1);
	tp_assert(0, "you need check MEM value");

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
