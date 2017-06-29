/* filemap.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "test_mck.h"
#include "testsuite.h"

const char* filemap_do_map(
	const char* path, int open_flags,
	int mmap_prot, int mmap_flags,
	void(*cb)(void*,char*,size_t), void* cb_arg)
{
	int status;
	int fd;
	struct stat sbuf;
	char* data;
	
	tp_assert(path != NULL, "invalid argument.");
	
	/* open */
	fd = open(path, open_flags);
	tp_assert(fd != -1, "file open error.");
	
	/* get size */
	status = stat(path, &sbuf);
	tp_assert(status != -1, "stat error.");
	
	/* mmap */
	data = (char*)mmap((caddr_t)0, sbuf.st_size,
			   mmap_prot, mmap_flags, fd, 0);
	tp_assert(data != (char*)-1, "mmap error.");
	
	/* do call back */
	cb(cb_arg, data, sbuf.st_size);
	
	/* munmap */
	status = munmap(data, sbuf.st_size);
	tp_assert(status != -1, "munmap error.");
	
	/* close */ 
	close(fd);
	
	return NULL;
}

void filemap_cb_write(void* arg, char* buf, size_t size)
{
	*buf = 'Z';
	printf("%c\n", *buf);
}

void filemap_cb_read(void* arg, char* buf, size_t sizs)
{
	char temp = *buf;
	printf("%c\n", temp);
}

void filemap_cb_read_write(void* arg, char* buf, size_t size)
{
	filemap_cb_read(arg, buf, size);
	filemap_cb_write(arg, buf, size);
}

static struct filemap_args param;

void* filemap_parse_args(int argc, char** argv)
{
	int opt;
	
	memset(&param, 0, sizeof(param));
	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':  /* file path */
			param.file = optarg;
			break;
		default:
			return NULL;
		}
	}
	
	if (param.file == NULL) {
		fprintf(stderr, "mandatory parameter '-f <file>'\n");
	}
	return &param;
}
