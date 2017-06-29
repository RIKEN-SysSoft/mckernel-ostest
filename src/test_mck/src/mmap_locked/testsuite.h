/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

/* mmap_locked.c */
struct mmap_locked_args {
	const char* file;
};

void* mmap_locked_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
