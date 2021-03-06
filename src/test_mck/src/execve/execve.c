/* execve.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <errno.h>
#include <sys/types.h>
#include "test_mck.h"
#include "testsuite.h"

static struct execve_args param;
void* execve_parse_args(int argc, char** argv)
{
	int opt; 
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f': 
			param.file_path = optarg; 
			break; 
		default: 
			return NULL;
		} 
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.file_path == NULL) {
		fprintf(stderr, "mandatory parameter '-f <executable-file>'\n"); 
	} 
	return &param;
}
