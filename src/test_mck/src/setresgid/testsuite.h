/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setresgid.c */
struct setresgid_args {
	gid_t rgid;
	gid_t egid;
	gid_t sgid;
	char **argv;
	int argc;
};

void *setresgid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
