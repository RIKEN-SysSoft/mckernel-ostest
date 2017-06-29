/* 015.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct rlimit set_rlim = {
		0xffffffffffffffff,	/* rlim_cur */
		0			/* rlim_max */
	};

	const int resource = RLIMIT_SIGPENDING;
	int result = 0;
	struct rlimit org_rlim;

	/* get original rlimit */
	result = getrlimit(resource, &org_rlim);
	tp_assert(result == 0, "getrlimit failed.");

	set_rlim.rlim_max = org_rlim.rlim_max;
	tp_assert(set_rlim.rlim_max < set_rlim.rlim_cur, "rlim_cur is less than rlim_max");

	/* set rlimit */
	result = setrlimit(resource, &set_rlim);
	printf("setrlimit() = %d, errno = %d\n", result, errno);
	tp_assert(result != 0, "setrlimit why successed ?");
	tp_assert(errno == EINVAL, "setrlimit errno is not EINVAL");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
