/* getegid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct getegid_args param;
void *getegid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "e:")) != -1) {
		switch (opt) {
		case 'e':
			param.egid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.egid == -1 || param.egid == 0) {
		fprintf(stderr, "mandatory parameter '-e <egid>'\n");
	}
	return &param;
}
