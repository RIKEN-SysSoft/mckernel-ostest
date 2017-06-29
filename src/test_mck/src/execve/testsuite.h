/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <unistd.h>
#include <string.h>
#include <errno.h>

/* execve.c */
struct execve_args {
	char *file_path;
	char **argv;
	int argc;
};

void* execve_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
