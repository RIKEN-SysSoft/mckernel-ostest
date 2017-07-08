/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <stdlib.h>

#include "test_mck.h"
#include "testsuite.h"

static volatile unsigned long threads_barrier = 0;

typedef struct {
	int local_id;
	int waiting;
} child_arg_t;

typedef struct {
	pthread_t thr;
	int iret;
	child_arg_t ch_args;
} child_thread_t;

void *child_func(void *arg)
{
	child_arg_t *c_arg = (child_arg_t *)arg; 
	c_arg->waiting = 1;

	while(threads_barrier == 0) {
		cpu_pause();
	}

	printf("[tid=%d] from child thread.(local_id=%d)\n", gettid(), c_arg->local_id);
	return NULL;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return cpu_thread_limits_get_param(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ith; 
	struct cpu_thread_limits_args *args = (struct cpu_thread_limits_args*)tc_arg;
	child_thread_t *child = NULL;
	struct rlimit rlimit_nproc;
	int ret;

	tp_assert(args != NULL, "internal error.");
	tp_assert(0 < args->thread_num, "-t <child thread num> invalid argument.");
	tp_assert(0 < args->cpu_num, "-c <num cpus> invalid argument.");

	child = calloc(args->thread_num, sizeof(child_thread_t));
	tp_assert(child != NULL, "calloc error.");

	getrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(rlimit_nproc.rlim_max >= args->cpu_num, "-c <num cpus> is larger than hard limit");
	rlimit_nproc.rlim_cur = args->cpu_num;
	ret = setrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(ret == 0, "setrlimit error");

	for (ith = 0; ith < args->thread_num; ith++) {
		child[ith].ch_args.local_id = ith+1; 
		child[ith].ch_args.waiting = 0;
		child[ith].iret = pthread_create(&child[ith].thr, NULL, child_func, &child[ith].ch_args); 
	} 

	for (ith = 0; ith < args->thread_num; ith++) {
		if (child[ith].iret != 0) {
			printf("failed to create thread[%d]: %d\n", ith, child[ith].iret);
			free(child);
			tp_assert(NULL, "pthread_create failed.");
		}
	}

	/* Wait for all threads are awoke. */
retry:
	for (ith = 0; ith < args->thread_num; ith++) {
		if(child[ith].ch_args.waiting == 0)
			goto retry;
	}

	threads_barrier = 1;

	for (ith = 0; ith < args->thread_num; ith++) {
		pthread_join(child[ith].thr, NULL);
	}
	free(child);
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
