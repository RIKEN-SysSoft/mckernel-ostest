/* setregid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct setregid_args param;
void *setregid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "r:e:")) != -1) {
		switch (opt) {
		case 'r':
			param.rgid = atoi(optarg);
			break;
		case 'e':
			param.egid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.rgid == -1 || param.rgid == 0) {
		fprintf(stderr, "mandatory parameter '-r <rgid>'\n");
	}
	if (param.egid == -1 || param.egid == 0) {
		fprintf(stderr, "mandatory parameter '-e <egid>'\n");
	}
	return &param;
}
