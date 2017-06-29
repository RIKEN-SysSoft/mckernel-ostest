/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>

struct wait4_args {
	char *filepath;
};

#endif /*__TEST_SUITE_H__*/
