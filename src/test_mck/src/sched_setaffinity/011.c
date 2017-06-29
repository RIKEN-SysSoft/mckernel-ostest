/* 011.c COPYRIGHT FUJITSU LIMITED 2015 */

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

struct child_thread {
	int num;
	pthread_t pthr;
	sem_t sync;
	sem_t start;
	int read_fd;
	int write_fd;
};

struct child_thread_info {
	int cpuid;
	pid_t tid;
};

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0);

static void child_fn_common(struct child_thread *data)
{
	// notify current cpu data
	struct child_thread_info ci;
	ci.cpuid = get_cpu_id();
	ci.tid = gettid();
	// printf("[child1] child thread1 work on cpu%d\n", ci.cpuid);
	write(data->write_fd, &ci, sizeof(struct child_thread_info));

	printf("==[tid:%d on cpu%d]==\n", (int)ci.tid, ci.cpuid);
	show_extended_registers();

	sem_post(&data->sync);
	// printf("[%d] wait\n", data->num);
	sem_wait(&data->start);
	// printf("[%d] start\n", data->num);

	/*
	 * 設定した値が復元されていることを確認
	 */
	printf("==[tid:%d on cpu%d]==\n", (int)gettid(), get_cpu_id());
	show_extended_registers();

	// close pipes.
	close(data->read_fd);
	close(data->write_fd);

}

static void *func_c1(void *value)
{
	struct child_thread* data = (struct child_thread*)value;

	unsigned long xg[] = { 0x00000000ffffffff, 0xffffffff00000000, 0x0000ffff0000ffff, 0xffff0000ffff0000 };
	float xf[] = { 11.11, 22.22, 33.33, 44.44 };

	/*
	 * 各種レジスタに値を設定
	 */
	write_xg(xg[0], xg[1], xg[2], xg[3]);
	write_xf(&xf[0], &xf[1], &xf[2], &xf[3]);

	child_fn_common(data);

	return NULL;
}

static void *func_c2(void *value)
{
	struct child_thread* data = (struct child_thread*)value;

	unsigned long xg[] = { 0x1111111111111111, 0xffffffffffffffff, 0xaaaaaaaaaaaaaaaa, 0x5555555555555555 };
	float xf[] = { 98.76, 54.32, 12.34, 56.78 };

	/*
	 * 各種レジスタに値を設定
	 */
	write_xg(xg[0], xg[1], xg[2], xg[3]);
	write_xf(&xf[0], &xf[1], &xf[2], &xf[3]);

	child_fn_common(data);

	return NULL;
}

#define CHILD_THREAD_COUNT 0x2UL

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int num_cpus;
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	struct child_thread children[CHILD_THREAD_COUNT];
	int result = -1;
	int pipe_fd[2];
	struct child_thread_info child_info[2] = {{0}};
	cpu_set_t *cpusetp_child;
	size_t size;

	/* validate */
	tp_assert(args->num_of_cpus > 0, "mandatory parameter '-p <num_of_cpus>'");
	num_cpus = args->num_of_cpus;
	
	/* init */
	for (i = 0; i < CHILD_THREAD_COUNT; i++) {
		/* create pipe */
		pipe(pipe_fd);

		/* create */
		children[i].num = i;
		children[i].read_fd = pipe_fd[0];
		children[i].write_fd = pipe_fd[1];
		sem_init(&children[i].sync, 0, 0);
		sem_init(&children[i].start, 0, 0);

		switch(i){
			case 0:
				pthread_create(&children[i].pthr, NULL, func_c1, &children[i]);
				break;
			case 1:
				pthread_create(&children[i].pthr, NULL, func_c2, &children[i]);
				break;
			default:
				break;
		}

		// Wait for notify
		read(pipe_fd[0], &child_info[i], sizeof(struct child_thread_info));
		// printf("[parent] child thread(tid:%d) work on cpu%d\n", child_info[i].tid, child_info[i].cpuid);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}

	// set child process affinity
	cpusetp_child = CPU_ALLOC(num_cpus);
	tp_assert(cpusetp_child != NULL, "(child process) alloc failed.");

	size = CPU_ALLOC_SIZE(num_cpus);
	CPU_ZERO_S(size, cpusetp_child);
	CPU_SET_S(child_info[0].cpuid, size, cpusetp_child);

	for (i = 0; i < CHILD_THREAD_COUNT; i++) {
		/* synchronization */
		sem_wait(&children[i].sync);
	}
	fflush(stdout);

	// set child2 affinity equals to child1
	result = sched_setaffinity(child_info[1].tid, size, cpusetp_child);
	CPU_FREE(cpusetp_child);

	/* start */
	sem_post(&children[1].start);
	sem_post(&children[0].start);

	for (i = 0; i < CHILD_THREAD_COUNT; i++) {
		/* destroy */
		pthread_join(children[i].pthr, NULL);
		sem_destroy(&children[i].sync);
		sem_destroy(&children[i].start);
	}

	if(result == -1){
		tp_assert(result != -1, "sched_setaffinity failed from to process.");
	}

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0);
