/* 003.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <linux/unistd.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define BUF_LEN 4096

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

void *child_thread_procfs_03(void *arg)
{
	int fd = 0;
	pid_t pid = getpid();
	pid_t tid = syscall(__NR_gettid);
	char pfname[PROCFILE_LEN];
	char buf[BUF_LEN];

	/* child thread function */
	/* procfile name generate */
	sprintf(pfname, "/proc/%d/task/%d/stat", pid, tid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* read */
	if ((read(fd, buf, BUF_LEN)) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("output %s\n", pfname);
	printf("%s\n", buf);

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}
	return 0;

/* error case */
read_err:
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
	}

open_err:
close_err:
	printf("TP failed.\n");
	return 0;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pthread_t child;
	int ret = 0;

	/* parent thread function */
	/* thread create */
	ret = pthread_create(&child, NULL, child_thread_procfs_03, NULL);
	if (ret) {
		printf("pthread_create failed. %d\n", errno);
		goto err;
	}

	/* wait thread */
	ret = pthread_join(child, NULL);
	if (ret) {
		printf("pthread_join failed. %d\n", errno);
	}

err:
	tp_assert(0, "you need check STAT value");
}
