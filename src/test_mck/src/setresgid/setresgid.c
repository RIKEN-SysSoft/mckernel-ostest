/* setresgid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static struct setresgid_args param;
void *setresgid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "r:e:s:")) != -1) {
		switch (opt) {
		case 'r':
			param.rgid = atoi(optarg);
			break;
		case 'e':
			param.egid = atoi(optarg);
			break;
		case 's':
			param.sgid = atoi(optarg);
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
	if (param.sgid == -1 || param.sgid == 0) {
		fprintf(stderr, "mandatory parameter '-s <sgid>'\n");
	}
	return &param;
}
