/* geteuid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct geteuid_args param;
void *geteuid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "e:")) != -1) {
		switch (opt) {
		case 'e':
			param.euid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.euid == -1 || param.euid == 0) {
		fprintf(stderr, "mandatory parameter '-e <euid>'\n");
	}
	return &param;
}
