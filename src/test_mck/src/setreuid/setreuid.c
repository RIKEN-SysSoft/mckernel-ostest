/* setreuid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct setreuid_args param;
void *setreuid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "r:e:")) != -1) {
		switch (opt) {
		case 'r':
			param.ruid = atoi(optarg);
			break;
		case 'e':
			param.euid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.ruid == -1 || param.ruid == 0) {
		fprintf(stderr, "mandatory parameter '-r <ruid>'\n");
	}
	if (param.euid == -1 || param.euid == 0) {
		fprintf(stderr, "mandatory parameter '-e <euid>'\n");
	}
	return &param;
}
