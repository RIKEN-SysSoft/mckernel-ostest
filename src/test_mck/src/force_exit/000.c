/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

struct force_exit_args gl_args;
SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int opt;
	memset(&gl_args, 0, sizeof(gl_args));

	while ((opt = getopt(tc_argc, tc_argv, "f:d:")) != -1) {
		switch (opt) {
		case 'f':
			gl_args.file_path = optarg;
			break;
		case 'd':
			gl_args.dev_path = optarg;
			break;
		default:
			fprintf(stderr, "unknown option. %c\n", (char)opt);
			return NULL;
		}
	}
	return &gl_args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct force_exit_args *args = NULL;
	int fd, ret = 0;
	FILE *fp;
	char buf[16];
	void *p = NULL;

	args = tc_arg;
	tp_assert(args != NULL, "invalid argument.");
	tp_assert(args->file_path != NULL, "invalid file path.");
	tp_assert(args->dev_path != NULL, "invalid dev path.");

	fp = fopen(args->file_path, "r");
	tp_assert(fp != NULL, "Faild Open file path.");

	ret = fread(buf, 1, 1, fp);
	tp_assert(ret == 1, "fread Failed.");
	printf("read 1 byte (%c(%d))\n", buf[0], buf[0]);

	fd = open(args->dev_path, O_RDWR);
	tp_assert(0 <= fd, "Faild Open dev path.");

	ioctl(fd, 50, NULL);
	p = mmap(NULL, 1024, PROT_NONE, MAP_PRIVATE, fd, 0);
	printf("mmap(%p)\n", p);

	while(1) {
		cpu_pause();
	}
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
