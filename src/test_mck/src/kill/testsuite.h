/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

extern int sig_count;

void kill_sig_handler(int sig);
void kill_child_fn(void);
void* kill_parse_args(int argc, char** argv);

struct kill_args {
	int proc_num;
};

#endif /*__TEST_SUITE_H__*/
