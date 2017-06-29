/* madvise.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <fcntl.h>
#include <string.h>
#include "test_mck.h"
#include "testsuite.h"

const char* madvise_simple(
	const char* path, int advise_flags,
	int expect_result, int expect_error)
{
	int i;
	int status;
	int fd; 
	struct stat sbuf;
	char* data;
	char *str_for_file = "abcdefghijklmnopqrstuvwxyz12345\n";
	ssize_t wrote_len;

	tp_assert(path != NULL, "invalid argument.");

	/* open */
	fd = open(path, O_RDWR | O_CREAT);
	tp_assert(fd != -1, "file open error.");

	/* Writing 40 KB of random data into this file
	   [32 * 1280 = 40960] */
	for (i = 0; i < 1280; i++) {
		wrote_len = write(fd, str_for_file, strlen(str_for_file));
		tp_assert(wrote_len != -1, "write failed");
	}

	/* get size */
	status = fstat(fd, &sbuf);
	tp_assert(status != -1, "stat error.");

	/* mmap */
	data = (char*)mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	tp_assert(data != (char*)-1, "mmap error.");

	/* madvise */
	errno = 0;
	status = syscall(__NR_madvise, data, sbuf.st_size, advise_flags);
	tp_assert(status == expect_result, "madvise error.");
	tp_assert(errno == expect_error, "madvise unexpected error occur.");

	/* munmap */
	status = munmap(data, sbuf.st_size);
	tp_assert(status != -1, "munmap error.");

	/* close */ 
	close(fd);

	return NULL;
}

static struct madvise_args param;

void* madvise_parse_args(int argc, char** argv)
{
	int opt;

	memset(&param, 0, sizeof(param));
	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			param.file = optarg;
			break;
		default:
			return NULL;
		}
	}

	if (param.file == NULL) {
		fprintf(stderr, "mandatory parameter '-f <mapping-file>'\n");
	}
	return &param;
}
