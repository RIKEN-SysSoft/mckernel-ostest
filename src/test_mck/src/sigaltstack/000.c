/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/types.h>

#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	stack_t sigstk, osigstk;	/* signal stack storing struct. */

	/* Call sigaltstack() to set up an alternate stack */
	sigstk.ss_size = SIGSTKSZ;
	sigstk.ss_flags = 0;
	sigaltstack(&sigstk, &osigstk);

	/* システムコール呼び出しから返ってくれば、テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
