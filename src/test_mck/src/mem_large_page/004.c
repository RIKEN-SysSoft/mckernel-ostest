/* 004.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "test_mck.h"
#include "testsuite.h"

static char* file_path;
static char* addr = (void*)-1;
static size_t length = 2 * LARGE_PAGE_SIZE;
static int fd = -1;
SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char* start = NULL;
	char* end = NULL;
	int large_page_flag = 0;
	
	/* open */
	file_path = test_mck_create_temp_file("mem_large_page_004", length);
	tp_assert(file_path != NULL, "create temp file error.");
	fd = open(file_path, O_RDONLY);
	tp_assert(fd != -1, "file open error.");
	
	/* mmap */
	addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
	tp_assert(addr != (void*)-1, "mmap error.");
	printf("mmap=%p\n", addr);
	
	/* access */
	start = (void*)align_up((unsigned long)addr, LARGE_PAGE_SIZE);
	end = (void*)((unsigned long)start + length / 2);
	tp_assert(start < end, "address overflow.");
	
	while (start < end) {
		unsigned long tte;
		unsigned long pgsz;
		/* load */
		char ch = *start; /* demand paging (page size = NORMAL) */
		(void)ch;
#if 0 /* for fx10 */
		/* check tte */
		tte = mck_va2pte((unsigned long)start);
		tp_assert(mck_pte_is_valid(tte) != 0, "tte valid.");
		/* check page size*/
		if (large_page_flag == 0) {
			pgsz = mck_pte_get_page_size(tte);
			if (pgsz != PAGE_SIZE) {
				large_page_flag = 1;
			}
		}
#else /* for fx100 */
		UNUSED_VARIABLE(tte);
		UNUSED_VARIABLE(pgsz);
		large_page_flag = check_page_size((unsigned long)start, PAGE_SIZE);
		if (!large_page_flag) {
			large_page_flag = 1;
			break;
		} else {
			large_page_flag = 0;
		}
#endif
		/* next */
		start += PAGE_SIZE;
	}
	// ファイルマッピングはノーマルページが期待
	tp_assert(large_page_flag == 0, "invalid page size.");
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (addr != (void*)-1) {
		munmap(addr, length);
	}
	if (fd != -1) {
		close(fd);
	}
	if (file_path) {
		test_mck_delete_temp_file(file_path);
	}
}
