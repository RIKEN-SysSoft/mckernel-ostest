/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include "test_mck.h"
#include "testsuite.h"

#include <pthread.h>

typedef struct process_id {
	pid_t pid;
	pid_t tid;
} process_id_t;

static void *child_thread_func(void* argument)
{
	process_id_t *c_args = (process_id_t *)argument;

	/* Get child thread ids. */
	c_args->pid = getpid();
	c_args->tid = gettid();
	return 0; 
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	process_id_t parent_info;

	pthread_t child_thread;
	process_id_t child_info;

	ret = pthread_create(&child_thread , NULL , child_thread_func , &child_info);
	tp_assert(ret == 0, "pthread_create error!!");

	/* Get parent thread ids. */
	parent_info.pid = getpid();
	parent_info.tid = gettid();
	printf("[parent] pid=%d, tid=%d\n", parent_info.pid, parent_info.tid);
	tp_assert(parent_info.tid == parent_info.pid, "TID of the parent thread is not equal to the PID!");

	ret = pthread_join(child_thread, NULL);
	tp_assert(ret == 0, "pthread_join error!!");

	printf("[child ] pid=%d, tid=%d\n", child_info.pid, child_info.tid);
	tp_assert(parent_info.tid != child_info.tid, "Child tid is not a unique!");

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
