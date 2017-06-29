/* mem_large_page.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define PAGEMAP_PAGE_SHIFT_BITS 0x1F80000000000000UL /* bits 55-60 */
#define PAGEMAP_PAGE_SHIFT_SHIFT 55UL

int check_page_size(unsigned long va, unsigned long pagesize)
{
	int fd = -1;
	int ret = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	unsigned long pagemap = 0;
	unsigned long pagemap_psize = 0;
	off_t offset = 0;

	/* procfile name generate */
	snprintf(pfname, sizeof(pfname), "/proc/%d/pagemap", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("%s open() failed. %d\n", pfname, errno);
		goto out;
	}

	/* calc offset */
	offset = va & PAGE_MASK;
	offset /= PAGE_SIZE;
	offset *= 8;

	/* lseek */
	if ((lseek(fd, offset, SEEK_SET)) == -1) {
		printf("%s lseek() failed. %d\n", pfname, errno);
		goto lseek_err;
	}

	/* read */
	if ((read(fd, &pagemap, sizeof(pagemap))) == -1) {
		printf("%s offset:%lx read() failed. %d\n", pfname, offset, errno);
		goto read_err;
	}

	/* check page size */
	/* page_shift is bits 55-60 */
	pagemap &= PAGEMAP_PAGE_SHIFT_BITS;
	pagemap >>= PAGEMAP_PAGE_SHIFT_SHIFT;
	pagemap_psize = 1UL << pagemap;

	if (pagemap_psize == pagesize) {
		ret = 1;
	} else {
		printf("%s pagesize = 0x%lx, Not as expected.\n", pfname, pagemap_psize);
	}
lseek_err:
read_err:
	/* close */
	if ((close(fd)) == -1) {
		printf("%s close() failed. %d\n", pfname, errno);
	}
out:
	return ret;
}
