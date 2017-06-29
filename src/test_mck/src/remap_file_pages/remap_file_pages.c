/* remap_file_pages.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
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

char* remap_file_pages_pgoff_to_addr(char *start, int page)
{
	return start + (PAGE_SIZE * page);
}

static unsigned char remap_file_pages_hash(int page)
{
	return (unsigned char)(page % 256);
}

const char* remap_file_pages_create_file(char *path, size_t size)
{
	int fd = -1;
	unsigned char buf[PAGE_SIZE] = {0};
	int page;
	
	fd = mkstemp(path);
	tp_assert(fd != -1, "open error.");
	
	for (page = 0; page < size/PAGE_SIZE; page++)
	{
		buf[0] = remap_file_pages_hash(page);
		write(fd, buf, sizeof(buf)/sizeof(buf[0]));
	}
	close(fd);
	sync();
	return NULL;
}

const char* remap_file_pages_compare(char *remap_data, size_t size)
{
	int remap_page;
	int orig_page;

	/* compare */
	remap_page = 0;
	orig_page  = (size / PAGE_SIZE) - 1;
	while (orig_page >= 0) {
		const char *remap_addr = remap_file_pages_pgoff_to_addr(remap_data, remap_page);
		unsigned char hash = remap_file_pages_hash(orig_page);
		tp_assert(*remap_addr == hash, "compare failed.");
		remap_page++;
		orig_page--;
	}
	return NULL;
}
