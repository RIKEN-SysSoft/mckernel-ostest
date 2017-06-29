/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

void write_xg(unsigned long xg0, unsigned long xg1, unsigned long xg2, unsigned long xg3);
void write_xf(float *xf0, float *xf1, float *xf2, float *xf3);
void show_extended_registers(void);

void* sched_setaffinity_parse_args(int argc, char** argv);
struct sched_setaffinity_args {
	int num_of_cpus;
};

#endif /*__TEST_SUITE_H__*/
