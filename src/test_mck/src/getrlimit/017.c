/* 017.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#define DELEGATE_NUMBER 0

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	getrlimit_tc tc[] = {
		/* resource,         res_str,             expect_result,          expect_errno, errno_str */
		{ RLIMIT_AS,         "RLIMIT_AS",         TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_CORE,       "RLIMIT_CORE",       TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_CPU,        "RLIMIT_CPU",        TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_DATA,       "RLIMIT_DATA",       TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_FSIZE,      "RLIMIT_FSIZE",      TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_LOCKS,      "RLIMIT_LOCKS",      TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_MEMLOCK,    "RLIMIT_MEMLOCK",    TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_MSGQUEUE,   "RLIMIT_MSGQUEUE",   TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_NICE,       "RLIMIT_NICE",       TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_NOFILE,     "RLIMIT_NOFILE",     TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_NPROC,      "RLIMIT_NPROC",      TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_RSS,        "RLIMIT_RSS",        TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_RTPRIO,     "RLIMIT_RTPRIO",     TEST_GETRLIMIT_SUCCESS, 0,            "0" },
//		{ RLIMIT_RTTIME,     "RLIMIT_RTTIME",     TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING", TEST_GETRLIMIT_SUCCESS, 0,            "0" },
		{ RLIMIT_STACK,      "RLIMIT_STACK",      TEST_GETRLIMIT_SUCCESS, 0,            "0" }
	};

	int result = 0;
	int i = 0;
	int failed = 0;
	struct rlimit rlim;
	const int res_num = sizeof(tc) / sizeof(tc[0]);

	tp_assert(0 <= tc_num, "invalid tp number.");

	for (i = 0; i < res_num; i++) {
		result = getrlimit(tc[i].resource, &rlim);
		printf("getrlimit(%s)=%d\n", tc[i].res_str, result);
		printf("  rlim_cur = %lx\n", rlim.rlim_cur);
		printf("  rlim_max = %lx\n", rlim.rlim_max);

		if(result != 0) {
			printf("  errno=%d (McK may return %s(=%d))\n",
				errno, tc[i].errno_str, tc[i].expect_errno);
		}

		if (result != tc[i].expect_result) {
			printf("  failed %s (McK may return %s(=%d))\n",
				tc[i].res_str, tc[i].errno_str, tc[i].expect_errno);
			failed = 1;
		}
	}
	tp_assert(failed == 0, "getrlimit failed.");
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)
