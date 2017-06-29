/* 003.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static char* buf[2];
static const int len = sizeof(buf)/sizeof(buf[0]);

static void sig_handler(int signame)
{
	if (signame == SIGSEGV) {
		exit(0);
	}
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	memset(buf, -1, sizeof(buf));
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	const int prot = PROT_READ;
	const int flags = MAP_PRIVATE|MAP_ANONYMOUS;
	int i;

	// mmap
	for (i = 0; i < len; i++) {
		buf[i] = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
		tp_assert(buf[i] != (void*)-1UL, "out of memory.(mmap)");
		printf("buf[%d]=%p\n", i, buf[i]);
	}

	// setup signal handler
	if (signal(SIGSEGV, sig_handler) == SIG_ERR) {
		tp_assert(NULL, "setup signal handler error.");
	}

	// non faulting load.
	if (memcmp(buf[0], buf[1], PAGE_SIZE)) {
	}
	if (memcmp(buf[1], buf[0], PAGE_SIZE)) {
	}
	// raise SIGSEGV
	if (buf[0] < buf[1]) {
		buf[1][PAGE_SIZE] = 'Z';
	}
	else {
		buf[0][PAGE_SIZE] = 'Z';
	}
	return "handle sigsegv error.";
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	for (i = 0; i < len; i++) {
		if (buf[i] != (void*)-1UL) {
			munmap(buf[i], PAGE_SIZE);
			buf[i] = (void*)-1UL;
		}
	}
}
