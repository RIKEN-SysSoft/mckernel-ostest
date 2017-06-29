/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <unistd.h>

struct cpu_thread_limits_args {
	int thread_num;
};
extern struct cpu_thread_limits_args *cpu_thread_limits_get_param(int argc, char *argv[]);

#endif /*__TEST_SUITE_H__*/
