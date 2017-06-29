/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setresuid.c */
struct getresuid_args {
	uid_t ruid;
	uid_t euid;
	uid_t suid;
	char **argv;
	int argc;
};

void *getresuid_parse_args(int argc, char **argv);

#endif /*__TEST_SUITE_H__*/
