/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <stdlib.h>
#include <pthread.h>

struct child {
	pthread_t tid;
	int num_cpus;
	void* errmsg;
};
static struct child* children;
static const int nr_children = 1;

static void* func(void* value)
{
	int status;
	struct child* child;
	pid_t tid;
	pthread_t self;
	cpu_set_t* sched_set_cpu;
	cpu_set_t* sched_get_cpu;
	cpu_set_t* pthread_get_cpu;
	size_t size;
	int vcpu;

	child = (struct child*)value;
	vcpu = get_cpu_id();
	if (vcpu < 0) {
		vcpu = 0;
	}
	tid = syscall(__NR_gettid);
	self = pthread_self();
	size = CPU_ALLOC_SIZE(child->num_cpus);

	//printf("vcpu %d, tid %d\n", vcpu, tid);

	/* sched_setaffinity */
	sched_set_cpu = CPU_ALLOC(child->num_cpus);
	tp_assert(sched_set_cpu != NULL, "CPU_ALLOC error.(sched_set)");
	CPU_ZERO_S(size, sched_set_cpu);

	CPU_SET_S(vcpu, size, sched_set_cpu);
	status = sched_setaffinity(tid, size, sched_set_cpu);
	tp_assert(status == 0, "sched_setaffinity error.");

	/* sched_getaffinity */
	sched_get_cpu = CPU_ALLOC(child->num_cpus);
	tp_assert(sched_get_cpu != NULL, "CPU_ALLOC error.(sched_get)");
	CPU_ZERO_S(size, sched_get_cpu);

	status = sched_getaffinity(tid, size, sched_get_cpu);
	tp_assert(status == 0, "sched_getaffinity error.");

	status = CPU_EQUAL_S(size, sched_set_cpu, sched_get_cpu);
	tp_assert(status != 0, "invalid affinity(sched_get)");

	/* pthread_getaffinity */
	pthread_get_cpu = CPU_ALLOC(child->num_cpus);
	tp_assert(pthread_get_cpu != NULL, "CPU_ALLOC error.(pthread_get)");
	CPU_ZERO_S(size, pthread_get_cpu);

	status = pthread_getaffinity_np(self, size, pthread_get_cpu);
	tp_assert(status == 0, "pthread_getaffinity_np error.");

	status = CPU_EQUAL_S(size, sched_set_cpu, pthread_get_cpu);
	tp_assert(status != 0, "invalid affinity(pthread_get)");

	return NULL;
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return pthread_getaffinity_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int num_of_cpus;

	struct pthread_getaffinity_args *args = (struct pthread_getaffinity_args*)tc_arg;

	tp_assert(0 < args->num_of_cpus, "mandatory parameter '-p <num_of_cpus>'");
	num_of_cpus = args->num_of_cpus;

	children = calloc(nr_children, sizeof(struct child));
	tp_assert(children != NULL, "calloc error.");

	for (i = 0; i < nr_children; i++) {
		children[i].num_cpus = num_of_cpus;
	}

	for (i = 0; i < nr_children; i++) {
		pthread_create(&children[i].tid, NULL, func, &children[i]);
	}
	for (i = 0; i < nr_children; i++) {
		pthread_join(children[i].tid, &children[i].errmsg);
		tp_assert(children[i].errmsg == NULL, children[i].errmsg);
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
