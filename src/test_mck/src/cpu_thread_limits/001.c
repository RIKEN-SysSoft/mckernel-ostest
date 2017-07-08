/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <stdlib.h>

#include "test_mck.h"
#include "testsuite.h"

static volatile unsigned long threads_barrier = 0;

static long numdiv = 200000000; 

typedef struct {
	long istart; 
	long iend; 
	double psum; 

	int local_id;
	int waiting;
} pi_arg_t;

typedef struct {
	pthread_t thr;
	int iret;
	pi_arg_t piargs;
} child_thread_t;

void *pi_calc(void *arg)
{
	pi_arg_t *piarg = (pi_arg_t *)arg; 
	long i; 
	double x, dx, sum=0.0; 

	// printf("id=%d\n", piarg->local_id);
	piarg->waiting = 1;

	while(threads_barrier == 0) {
		cpu_pause();
	}

	dx = 1.0 / (double)numdiv; 
	for (i = piarg->istart; i < piarg->iend; i++) {
		x = ((double)i + 0.5) * dx; 
		sum = sum + 4.0 / (1.0 + x*x); 
	} 
	piarg->psum = sum * dx; 
	return NULL;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return cpu_thread_limits_get_param(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ith;
	double pi = 0.0;
	struct cpu_thread_limits_args *args = (struct cpu_thread_limits_args*)tc_arg;
	child_thread_t *child = NULL;
	struct rlimit rlimit_nproc;
	int ret;

	tp_assert(args != NULL, "internal error.");
	tp_assert(0 < args->thread_num, "-t <child thread num> invalid argument.");
	tp_assert(0 < args->cpu_num, "-c <num cpus> invalid argument.");

	getrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(rlimit_nproc.rlim_max >= args->cpu_num, "-c <num cpus> is larger than hard limit");
	rlimit_nproc.rlim_cur = args->cpu_num;
	ret = setrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(ret == 0, "setrlimit error");

	child = calloc(args->thread_num, sizeof(child_thread_t));
	tp_assert(child != NULL, "calloc error.");

	for (ith = 0; ith < args->thread_num; ith++) {
		child[ith].piargs.istart = ith * numdiv / args->thread_num;
		child[ith].piargs.iend = (ith + 1) * numdiv / args->thread_num;
		child[ith].piargs.psum = 0.0;
		child[ith].piargs.local_id = ith+1;
		child[ith].piargs.waiting = 0;
		child[ith].iret = pthread_create(&child[ith].thr, NULL, pi_calc, &child[ith].piargs);
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
		if(child[ith].piargs.waiting == 0)
			goto retry;
	}

	threads_barrier = 1;

	for (ith = 0; ith < args->thread_num; ith++) {
		pthread_join(child[ith].thr, NULL);
		pi += child[ith].piargs.psum;
	}

	printf("PI = %16.14f\n", pi);
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
