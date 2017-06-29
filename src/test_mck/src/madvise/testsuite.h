/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <errno.h>

#define TEST_MADV_SUCCESS  0
#define TEST_MADV_FAILUER -1

/* madvise.c */
struct madvise_args {
	const char* file;
};

const char* madvise_simple(
	const char* path, int advise_flags,
	int expect_result, int expect_error);
void* madvise_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
