/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static __thread long tls = 0;
static long notls = 0;

static void* func(void *value)
{
    tls++;
    notls++;
    
    printf("[%ld] tls=%ld, notls=%ld\n",
           (long)value, tls, notls);
    return (void*)tls;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
    int i;
    struct {
        pthread_t tid;
        void* rc;
    } children[2] = {{0}};

    for (i = 0; i < sizeof(children)/sizeof(children[0]); i++) {
        /* create */
        pthread_create(&children[i].tid, NULL, func, (void*)(long)i);
        /* join */
        pthread_join(children[i].tid, &children[i].rc);
        /* check tls value */
        tp_assert(children[i].rc == (void*)1, "invalid TLS value.");
    }
	return NULL;
}
