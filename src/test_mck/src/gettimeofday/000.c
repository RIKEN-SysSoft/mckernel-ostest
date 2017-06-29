/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct timeval set, get;

	/* set the time */
	set.tv_sec = 6;
	set.tv_usec = 6;
	errno = 0;
	settimeofday(&set, NULL);
	tp_assert(errno == 0, "settimeofday error.");

	/* get the time */
	errno = 0;
	gettimeofday(&get, NULL);
	tp_assert(errno == 0, "gettimeofday error.");
	tp_assert(get.tv_sec <= (set.tv_sec + 1), "gotten timeval is too high.");
	tp_assert((get.tv_sec > set.tv_sec) ||
		  (get.tv_sec == set.tv_sec && get.tv_usec > set.tv_usec),
		  "gotten timecal is too low");

	printf("set = {tv_sec = %ld, tv_usec = %ld}\n", set.tv_sec, set.tv_usec);
	printf("get = {tv_sec = %ld. tv_usec = %ld}\n", get.tv_sec, get.tv_usec);

	/* test succeeeds */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
