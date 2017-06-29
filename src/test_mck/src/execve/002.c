/* 002.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <libgen.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	char *argv[2];
	extern char **environ;

	argv[0] = "UnknownAppFileName";
	argv[1] = NULL;

	ret = execve("UnknownAppFileName", argv, environ);

	printf("[%s] ret=%d, errno=%d\n", __func__, ret, errno);

	tp_assert(ret == -1, "execve return unknown code");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
