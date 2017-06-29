/* getuid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct getuid_args param;
void *getuid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "u:")) != -1) {
		switch (opt) {
		case 'u':
			param.uid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.uid == -1 || param.uid == 0) {
		fprintf(stderr, "mandatory parameter '-u <uid>'\n");
	}
	return &param;
}
