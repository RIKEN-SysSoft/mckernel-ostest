/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setfsuid.c */
struct setfsuid_args {
	uid_t fsuid;
	char **argv;
	int argc;
};

void *setfsuid_parse_args(int argc, char **argv);
int check_procfs_status_uid(uid_t set_fsuid);

#endif /*__TEST_SUITE_H__*/
