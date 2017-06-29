/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>

/* setfsgid.c */
struct setfsgid_args {
	gid_t fsgid;
	char **argv;
	int argc;
};

void *setfsgid_parse_args(int argc, char **argv);
int check_procfs_status_gid(gid_t set_fsgid);

#endif /*__TEST_SUITE_H__*/
