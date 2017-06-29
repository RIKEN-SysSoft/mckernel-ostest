/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
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
#define BUF_LEN 1024

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd = 0, i = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	unsigned long buf[BUF_LEN];

	sprintf(pfname, "/proc/%d/auxv", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto err;
	}

	/* read */
	if ((read(fd, buf, sizeof(buf))) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s:\n", pfname);
	for (i = 0; i < BUF_LEN / 2; i += 2) {
		printf("  %#016lx %#016lx (%s)\n", buf[i], buf[i + 1], auxv_name[buf[i]]);
		if (buf[i] == 0 && buf[i + 1] == 0) {
			break;
		}
	}

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto err;
	}
	tp_assert(0, "you need check AUXV value");

read_err:
	/* error close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
	}
err:
	/* tp error */
	tp_assert(0, "TP failed.");
}
