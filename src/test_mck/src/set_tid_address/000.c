/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <sys/types.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <errno.h>

#include "test_mck.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

int clear_tid = -1;

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long ret = 0;
	pid_t pid = getpid();

	/* set_tid_addres send */
	ret = syscall(__NR_set_tid_address, &clear_tid);

	/* check */
	tp_assert(ret == pid, "set_tid_address() failed.\n");

	return NULL;
}
