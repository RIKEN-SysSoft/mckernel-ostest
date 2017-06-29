/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <sys/types.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h> 

struct pthread_getaffinity_args {
	int num_of_cpus;
};
void* pthread_getaffinity_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
