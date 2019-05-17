/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>

void* sched_getaffinity_parse_args(int argc, char** argv);
struct sched_getaffinity_args {
	int num_of_cpus;
	char *file_path;
	char *cpuset_path; /* for 006.c */
	char **argv;
	int argc;
};

#endif /*__TEST_SUITE_H__*/
