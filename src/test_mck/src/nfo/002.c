/* 002.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "test_mck.h"
#include "testsuite.h"

static __thread char* buf[2];
static const int len = sizeof(buf)/sizeof(buf[0]);

static sem_t sync;
static sem_t start;

static void* func(void* arg)
{
	const int prot = PROT_READ;
	const int flags = MAP_PRIVATE|MAP_ANONYMOUS;
	int i;
	
	memset(buf, -1, sizeof(buf));

	for (i = 0; i < len; i++) {
		buf[i] = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
		tp_assert(buf[i] != (void*)-1UL, "out of memory.(mmap)");
		printf("mmap:buf[%d]=%p\n", i, buf[i]);
	}

	// non faulting load.
	if (memcmp(buf[0], buf[1], PAGE_SIZE)) {
	}
	if (memcmp(buf[1], buf[0], PAGE_SIZE)) {
	}
	return NULL;
}

static void* child(void* arg)
{
	sem_post(&sync);
	sem_wait(&start);
	return func(arg);
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pthread_t thread;
	char* ret;
	int state = 0;

        sem_init(&sync, 0, 0);
        sem_init(&start, 0, 0);
	state = pthread_create(&thread, NULL, child, NULL);
	tp_assert(state == 0, "pthread_create error.");

	// parent:NFO
	sem_wait(&sync);
	ret = func(NULL);
	tp_assert(ret == NULL, ret);

	// child:NFO
	sem_post(&start);
	state = pthread_join(thread , (void**)&ret);
	tp_assert(state == 0, "pthread_join error.");
	tp_assert(ret == NULL, ret);
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
