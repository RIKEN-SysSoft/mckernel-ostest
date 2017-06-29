/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setreuid.c */
struct setreuid_args {
	uid_t ruid;
	uid_t euid;
	char **argv;
	int argc;
};

void *setreuid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
