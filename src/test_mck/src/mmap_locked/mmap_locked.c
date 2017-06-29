/* mmap_locked.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

static struct mmap_locked_args param;

void* mmap_locked_parse_args(int argc, char** argv)
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
