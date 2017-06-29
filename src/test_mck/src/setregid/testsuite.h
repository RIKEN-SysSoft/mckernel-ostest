/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setregid.c */
struct setregid_args {
	gid_t rgid;
	gid_t egid;
	char **argv;
	int argc;
};

void *setregid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
