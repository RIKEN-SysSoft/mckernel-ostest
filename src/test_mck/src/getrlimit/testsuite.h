/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <errno.h>
#include <sys/resource.h>

#define TEST_GETRLIMIT_SUCCESS  0
#define TEST_GETRLIMIT_FAILUER -1

typedef struct getrlimit_testcase {
	int resource;
	char *res_str;
	int expect_result;
	int expect_errno;
	char *errno_str;
} getrlimit_tc ;


#endif /*__TEST_SUITE_H__*/
