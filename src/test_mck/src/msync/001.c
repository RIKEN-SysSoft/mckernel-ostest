/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
/*
 * msync(..., MS_INVALIDATE)で他のマッピングに通知ができること
 */
#include <pthread.h>
#include "test_mck.h"
#include "testsuite.h"

enum {
	STAT_NULL,
	STAT_MSYNC,
	STAT_CHECK_MEM,
};
static int status = STAT_NULL;

static char* tmp_file = NULL;

#define LENGTH (PAGE_SIZE * 3)
static const size_t length = LENGTH;
static const int prot = PROT_WRITE | PROT_READ;
static const int flags = MAP_SHARED;

static void* func(void* arg)
{
	int ret;
	int fd = -1;
	void* addr = MAP_FAILED;

	/* open */
	fd = open(tmp_file, O_RDWR);
	tp_assert(fd != -1, "open error");

	/* map */
	addr = mmap(NULL, length, prot, flags, fd, 0);
	tp_assert(addr != MAP_FAILED, "mmap error.");

	/* write */
	memset(addr, '1', length);

	/* msync */
	ret = msync(addr, length, MS_SYNC|MS_INVALIDATE);
	tp_assert(ret != -1, "msync error.");
	status = STAT_MSYNC;
	while (status != STAT_CHECK_MEM) {
		cpu_pause();
	}

	/* unmap */
	if (addr != MAP_FAILED) {
		munmap(addr, length);
	}

	/* delete temp file */
	if (fd != -1) { 
		close(fd);
	}
	return NULL;
}


SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pthread_t thread;
	int ret;
	int fd = -1;
	char* addr = MAP_FAILED;
	char buf[LENGTH];
	size_t size;
	int i;

	/*--- initializse ---*/
	/* create tmep file */
	tmp_file = test_mck_create_temp_file(TEMP_FILE_PREFIX, length);
	tp_assert(tmp_file != NULL, "create temp file error.");

	/* initialize file */
	fd = open(tmp_file, O_RDWR);
	tp_assert(fd != -1, "open error");

	memset(buf, '0', sizeof(buf));
	size = write(fd, buf, sizeof(buf));
	tp_assert(size == sizeof(buf), "write error.");

	ret = fsync(fd);
	tp_assert(ret != -1, "fsync error.");

	ret = close(fd);
	fd = -1;
	tp_assert(ret != -1, "close error.");

	/*--- do test ---*/
	/* open */
	fd = open(tmp_file, O_RDWR);
	tp_assert(fd != -1, "open error");

	/* map */
	addr = mmap(NULL, length, prot, flags, fd, 0);
	tp_assert(addr != MAP_FAILED, "mmap error.");

	/* create thread */
	ret = pthread_create(&thread, NULL, func, NULL);
	tp_assert(ret == 0, "pthread_create error.");

	while(status != STAT_MSYNC) {
		cpu_pause();
	}

	/* check */
	for (i = 0; i < LENGTH; i++) {
		tp_assert(addr[i] == '1', "memcmp error.");
	}
	status = STAT_CHECK_MEM;

	/*--- finalize ---*/
	/* join thread */
	ret = pthread_join(thread, NULL);
	tp_assert(ret == 0, "pthread_join error.");

	/* unmap */
	if (addr != MAP_FAILED) {
		munmap(addr, length);
	}

	/* delete temp file */
	if (fd != -1) { 
		close(fd);
	}

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* delete temp file */
	if (tmp_file != NULL) { 
		test_mck_delete_temp_file(tmp_file);
	}
}
