/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* getgid.c */
struct getgid_args {
	gid_t gid;
	char **argv;
	int argc;
};

void *getgid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
