/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <fcntl.h>           /* AT_* 定数の定義 */
#include <unistd.h>
#include <string.h>

struct readlinkat_args {
	char *file_path;
	char *link_path;
};

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct readlinkat_args param;
	int opt;

	memset(&param, 0 ,sizeof(param));

	while ((opt = getopt(tc_argc, tc_argv, "f:l:")) != -1) {
		switch (opt) {
		case 'f':
			param.file_path = optarg;
			break;
		case 'l':
			param.link_path = optarg;
			break;
		default:
			break;
		}
	}
	if ((param.file_path == NULL) || (param.link_path == NULL)) {
		fprintf(stderr, "mandatory parameter '-f <target-absolute-filepath> -l <link-filepath>'\n"); 
	} 
	return &param;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	ssize_t result = 0;
	char buf[1024];
	struct readlinkat_args* args;

	args = tc_arg;
	tp_assert(args != NULL, "invalid arguement.");
	tp_assert(args->file_path != NULL, "invalid file path.");
	tp_assert(args->link_path != NULL, "invalid file path.");

	printf("[INFO]readlinkat test\n target file:%s\n link file:%s\n",
		args->file_path, args->link_path);

	memset(buf, 0, sizeof(buf));

	result = readlinkat(AT_FDCWD, args->link_path, buf, sizeof(buf));
	tp_assert(result != 0, "readlinkat error.");
	printf(" buffer data:%s\n", buf);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
