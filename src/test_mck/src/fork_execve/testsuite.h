/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <unistd.h>
#include <string.h>
#include <errno.h>

struct fork_execve_args {
	char *file_path;
	char **argv;
	int argc;
};

/* fork_execve.c */
void* fork_execve_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
