/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <unistd.h>
#include <string.h>
#include <errno.h>

struct vfork_args {
	char *file_path;
	char **argv;
	int argc;
};

/* vfork.c */
void* vfork_parse_args(int argc, char** argv);
extern int vfork_global;

#endif /*__TEST_SUITE_H__*/
