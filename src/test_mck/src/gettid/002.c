/* 002.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static int child_cpu = -1;

struct child_thread {
	int num;
	pthread_t pthread;
	pid_t pre_pid;
	pid_t post_pid;
	pid_t pre_tid;
	pid_t post_tid;
	int pre_cpu;
	int post_cpu;
	sem_t sync;
	sem_t start;
};

static void* func(void *value)
{
	struct child_thread* data = (struct child_thread*)value;

	data->pre_cpu = sched_getcpu();
	data->pre_pid = getpid();
	data->pre_tid = gettid();
	if (child_cpu == -1) {
		child_cpu = sched_getcpu();
		sem_post(&data->sync);
	} else {
		cpu_set_t cpuset;
		pthread_t thread;
		int stat;
		
		thread = pthread_self();
		CPU_ZERO(&cpuset);
		CPU_SET(child_cpu, &cpuset);

		sem_post(&data->sync);
		stat = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
		tp_assert(stat == 0, "pthread_setaffinity_np error.");
	}
	sem_wait(&data->start);

	data->post_cpu = sched_getcpu();
	data->post_pid = getpid();
	data->post_tid = gettid();

	printf("child#%d cpu:%d,%d pid:%d,%d tid:%d,%d\n",
	       data->num,
	       data->pre_cpu, data->post_cpu,
	       data->pre_pid, data->post_pid,
	       data->pre_tid, data->post_tid);
	return NULL;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long t = -1;
	int opt;
	
	while ((opt = getopt(tc_argc, tc_argv, "t:")) != -1) {
		switch (opt) {
		case 't':  /* child thread */
			t = atol(optarg);
			break;
		default:
			return NULL;
		}
	}
	return (void*)t;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i, j;
	long child_count = (long)tc_arg;
	struct child_thread* children;
	
	/* validate */
	tp_assert(0 < child_count, "mandatory parameter '-t <child_thread_count>'\n");
	
	/* init */
	children = calloc(child_count, sizeof(struct child_thread));
	tp_assert(children != NULL, "calloc error.");	
	for (i = 0; i < child_count; i++) {
		children[i].num = i;
		sem_init(&children[i].sync, 0, 0);
		sem_init(&children[i].start, 0, 0);
	}
	
	for (i = 0; i < child_count; i++) {
		int state = pthread_create(&children[i].pthread, NULL, func, &children[i]);
		tp_assert(state == 0, "pthread_create error.");
		/* wait for child. */
		sem_wait(&children[i].sync);
		/* delay */
		sleep(1);
	}
	
	for (i = 0; i < child_count; i++) {
		/* start */
		sem_post(&children[i].start);
	}

	for (i = 0; i < child_count; i++) {
		/* destroy */
		pthread_join(children[i].pthread, NULL);
		sem_destroy(&children[i].sync);
		sem_destroy(&children[i].start);
	}
	fflush(stdout);
	
	for (i = 0; i < child_count - 1; i++) {
		/* check */
		tp_assert(children[i].pre_pid == children[i].post_pid, "PID is not match.");
		tp_assert(children[i].pre_tid == children[i].post_tid, "TID is not match.");
		for (j = i + 1; j < child_count; j++) {
			tp_assert(children[i].pre_pid == children[j].post_pid, "PID is not match.");
			tp_assert(children[i].pre_tid != children[j].post_tid, "Detected TID collision.");
		}
	}
	free(children);
	return NULL;
}
