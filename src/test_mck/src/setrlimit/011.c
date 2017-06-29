/* 011.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct rlimit set_rlim = {
		0xe0000000,	/* rlim_cur */
		0		/* rlim_max */
	};

	const int resource = RLIMIT_RSS;
	char *res_name = "RLIMIT_RSS";
	int result = 0;
	struct rlimit org_rlim, cmp_rlim;

	/* get original rlimit */
	result = getrlimit(resource, &org_rlim);
	tp_assert(result == 0, "getrlimit failed.");

	set_rlim.rlim_max = org_rlim.rlim_max;

	/* set rlimit */
	result = setrlimit(resource, &set_rlim);
	tp_assert(result == 0, "setrlimit failed.");

	/* get rlimit */
	result = getrlimit(resource, &cmp_rlim);
	tp_assert(result == 0, "getrlimit failed.");

	printf("  getrlimit(%s, %lx, %lx)\n", res_name, org_rlim.rlim_cur, org_rlim.rlim_max);
	printf("  setrlimit(%s, %lx, %lx)\n", res_name, set_rlim.rlim_cur, set_rlim.rlim_max);
	printf("  getrlimit(%s, %lx, %lx)\n", res_name, cmp_rlim.rlim_cur, cmp_rlim.rlim_max);
	tp_assert(set_rlim.rlim_cur == cmp_rlim.rlim_cur, "setrlimit failed, rlim_cur not set.");
	tp_assert(set_rlim.rlim_max == cmp_rlim.rlim_max, "setrlimit failed, rlim_max not set.");

	/* restore rlimit */
	result = setrlimit(resource, &org_rlim);
	tp_assert(result == 0, "setrlimit failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
