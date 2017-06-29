/* 005.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <linux/unistd.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define DUMP_SIZE 256

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

void *child_thread_procfs_05(void *arg)
{
	int fd = 0, i = 0;
	int space_flag = 0;
	pid_t pid = getpid();
	pid_t tid = syscall(__NR_gettid);
	char pfname[PROCFILE_LEN];

	const int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE;
	char *buf = NULL;
	const int prot1 = PROT_WRITE | PROT_READ;
	const int prot2 = PROT_WRITE | PROT_READ | PROT_EXEC;
	off_t dump_off = 0;
	off_t ret = 0;
	unsigned long read_buf[PAGE_SIZE * 2 / sizeof(unsigned long)];
	unsigned long *p = NULL;

	/* mmap buf (none) */
	buf = mmap(NULL, PAGE_SIZE * 2, PROT_NONE, flags, -1, 0);
	if (buf == (void *)-1) {
		tp_assert(0, "mmap none failed.");
	}
	printf("mmaped buf: %#016lx\n", (off_t)buf);

	/* fixed map(RW-) */
	p = mmap(buf, PAGE_SIZE, prot1, flags | MAP_FIXED, -1, 0);
	if (p == (void *)-1) {
		tp_assert(0, "mmap1(RW-) failed.");
	}
	printf("mmaped buf(RW-): %#016lx\n", (off_t)p);

	/* filled magic number */
	for (i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++) {
		p[i] = ~i;
	}

	/* fixed map(RWX) */
	p = mmap(buf + PAGE_SIZE, PAGE_SIZE, prot2, flags | MAP_FIXED, -1, 0);
	if (p == (void *)-1) {
		tp_assert(0, "mmap2(RWX) failed.");
	}
	printf("mmaped buf(RWX): %#016lx\n", (off_t)p);

	/* filled magic number */
	for (i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++) {
		p[i] = i;
	}

	sprintf(pfname, "/proc/%d/task/%d/mem", pid, tid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* lseek */
	if ((ret = lseek(fd, (off_t)buf, SEEK_SET)) == -1) {
		printf("lseek() failed. %d\n", errno);
		goto lseek_err;
	}

	/* read */
	if ((read(fd, read_buf, PAGE_SIZE * 2)) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s(offset:%#016lx):\n", pfname, (off_t)buf);
	dump_off = (off_t)buf;
	i = 0;
	while((dump_off - (off_t)buf) < PAGE_SIZE * 2) {
		/* dump print near the start, range boundary and end */
		if ((dump_off - (off_t)buf) < DUMP_SIZE ||
		    (PAGE_SIZE - DUMP_SIZE / 2 <= (dump_off - (off_t)buf) && (dump_off - (off_t)buf) < PAGE_SIZE + DUMP_SIZE / 2) ||
		    (PAGE_SIZE * 2 - DUMP_SIZE <= (dump_off - (off_t)buf))) {
			printf("  %#016lx: %016lx %016lx %016lx %016lx\n",
				dump_off, read_buf[i], read_buf[i + 1], read_buf[i + 2], read_buf[i + 3]);
			space_flag = 1;
		} else if (space_flag) {
			printf("  *\n");
			space_flag = 0;
		}
		i += 4;
		dump_off += sizeof(unsigned long) * 4;
	}

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}

	/* successed end */
	return 0;

/* error case */
lseek_err:
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

	ret = pthread_create(&child, NULL, child_thread_procfs_05, NULL);
	if (ret) {
		printf("pthread_create failed. %d\n", errno);
		goto err;
	}

	ret = pthread_join(child, NULL);
	if (ret) {
		printf("pthread_join failed. %d\n", errno);
	}

err:
	tp_assert(0, "you need check MEM value");
}
