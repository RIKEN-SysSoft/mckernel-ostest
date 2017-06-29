/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <string.h>
#include <pthread.h>
#include "arch_test_fpregs.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static void* fpregs002_child_fn(void* arg)
{
	printf("[child] after pthread_create.\n");
	show_current_fpregs();
	return 0;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int result;
	struct fpregs fregs0;
	pthread_t thread;

	memcpy(&fregs0, FPREGS_TEST_DATA, sizeof(struct fpregs));

	printf("[parent] before pthread_create.\n");
	fpregs_write(&fregs0);
	show_current_fpregs();

	result = pthread_create(&thread, NULL, fpregs002_child_fn, NULL);
	tp_assert(result == 0, "pthread_create error.");

	result = pthread_join(thread , NULL);
	tp_assert(result == 0, "pthread_join error.");

	// printf("[parent] after pthread_join\n");
	// show_current_fpregs();

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
