/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = getpid();
	pid_t tid = gettid();

	// シングルスレッドならば、 pid == tidのはず
	printf("PID=%d, TID=%d\n", pid, tid);
	tp_assert(pid == tid, "PID and TID do not match." );

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
