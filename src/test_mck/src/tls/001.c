/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static __thread long tls = 0;
static long notls = 0;

static const long INC_COUNT = 10000;

struct child_thread {
    int num;
    pthread_t tid;
    sem_t sync;
    sem_t start;
    long tls;
};

static void* func(void *value)
{
    unsigned long i;
    struct child_thread* data = (struct child_thread*)value;

    sem_post(&data->sync);
    printf("[%d] wait\n", data->num);
    sem_wait(&data->start);
    printf("[%d] start\n", data->num);

    for (i = 0; i < INC_COUNT; i++) {
        tls++;
        notls++;
    }
    data->tls = tls;

    printf("[%d] tls=%ld, notls=%ld\n",
           data->num, data->tls, notls);
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
    int i;
    long child_count = (long)tc_arg;
    struct child_thread* children;

    /* validate */
    tp_assert(0 < child_count, "mandatory parameter '-t <child_thread_count>'\n");

    /* init */
    children = calloc(child_count, sizeof(struct child_thread));
    tp_assert(children != NULL, "calloc error.");

    for (i = 0; i < child_count; i++) {
        /* create */
        children[i].num = i;
        sem_init(&children[i].sync, 0, 0);
        sem_init(&children[i].start, 0, 0);
        pthread_create(&children[i].tid, NULL, func, &children[i]);
    }

    for (i = 0; i < child_count; i++) {
        /* synchronization */
        sem_wait(&children[i].sync);
    }
    fflush(stdout);

    for (i = 0; i < child_count; i++) {
        /* start */
        sem_post(&children[i].start);
    }

    for (i = 0; i < child_count; i++) {
        /* destroy */
        pthread_join(children[i].tid, NULL);
        sem_destroy(&children[i].sync);
        sem_destroy(&children[i].start);
    }

    for (i = 0; i < child_count; i++) {
        /* check tls value */
        tp_assert(children[i].tls == INC_COUNT,
                  "invalid TLS value.");
    }

    free(children);
	return NULL;
}
