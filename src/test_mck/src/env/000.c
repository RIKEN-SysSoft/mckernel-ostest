/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

extern char** environ;
RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;

	for (i = 0; environ[i]; i++) {
		printf("%s\n", environ[i]);
	}
	return NULL;
}

