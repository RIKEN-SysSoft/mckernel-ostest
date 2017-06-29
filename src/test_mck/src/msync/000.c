/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
/*
 * msync(..., MS_SYNC)で同期可能なこと
 */
#include "test_mck.h"
#include "testsuite.h"

#define LENGTH (PAGE_SIZE * 3)

static char* tmp_file = NULL;
static int fd = -1;
static void* addr = MAP_FAILED;
static size_t length = LENGTH;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int prot = PROT_WRITE | PROT_READ;
	int flags = MAP_SHARED | MAP_LOCKED | MAP_POPULATE;
	char buf[LENGTH];
	char readbuf[LENGTH];
	size_t size;
	int ret;

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

	/* access */
	memset(addr, '1', length);
	memset(buf, '1', sizeof(buf));

	/* sync */
	ret = msync(addr, length, MS_SYNC);   
	tp_assert(ret != -1, "msync error.");

	/* check */
	size = read(fd, readbuf, sizeof(readbuf));
	tp_assert(size == sizeof(readbuf), "write error.");

	ret = memcmp(buf, readbuf, sizeof(readbuf));
	tp_assert(ret == 0, "memcmp error.");

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* unmap */
	if (addr != MAP_FAILED) {
		munmap(addr, length);
	}

	/* delete temp file */
	if (fd != -1) { 
		close(fd);
	}

	/* delete temp file */
	if (tmp_file != NULL) { 
		test_mck_delete_temp_file(tmp_file);
	}
}
