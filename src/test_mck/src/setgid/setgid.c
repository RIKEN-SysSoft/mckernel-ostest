/* setgid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct setgid_args param;
void *setgid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "g:")) != -1) {
		switch (opt) {
		case 'g':
			param.gid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.gid == -1 || param.gid == 0) {
		fprintf(stderr, "mandatory parameter '-g <gid>'\n");
	}
	return &param;
}
