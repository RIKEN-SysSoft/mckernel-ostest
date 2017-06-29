/* 004.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#ifndef MAX_NR_CPUS
#define MAX_NR_CPUS 1024
#endif

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0);

int primitive_ceil(int x, int y)
{
	int ret = 0;
	if(y != 0) {
		ret = x/y;
		if(x%y) ret++;
	}

	return ret;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_getaffinity_args *args = (struct sched_getaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int expect_error;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");
	tp_assert(args->num_of_cpus <= MAX_NR_CPUS, "value too big.(<= 1024)");

	cpusetp = CPU_ALLOC(MAX_NR_CPUS);
	tp_assert(cpusetp != NULL, "alloc failed.");

	/*
  	 * size : While having a bit of only can express the number of cores, 
  	 *        and is not a value in multiples of 8.
  	 *
  	 * if nr_cpus equals to 64, then sched_setaffinity will return success.
  	 */
	size = primitive_ceil(args->num_of_cpus, sizeof(unsigned long));
	expect_error = ((int)size) % sizeof(unsigned long) ? -1 : 0;

	CPU_ZERO_S(size, cpusetp);

	result = sched_getaffinity(0, size, cpusetp);
	CPU_FREE(cpusetp);

	tp_assert(result == expect_error, "sched_getaffinity returns unexpected error.");
	if(result != 0){
		printf("[size=%d] sched_getaffinity result:%d, errno:%d\n", (int)size, result, errno); 
		tp_assert(errno == EINVAL, "unexpected result is occured."); 
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
