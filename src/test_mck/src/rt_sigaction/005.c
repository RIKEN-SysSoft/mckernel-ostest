/* 005.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#define DELEGATE_NUMBER 0 /* both SETUP&TEARDOWN empty */
SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sigaction act;
	int i;
	int result;

	int err_sig[4] = {-1, 65, SIGKILL, SIGSTOP};

	act.sa_handler = SIG_DFL;
	for(i = 0; i < (sizeof(err_sig)/sizeof(err_sig[0])); i++) {
		result = sigaction(err_sig[i], &act, NULL);
		printf("sigaction(%d) = %d (errno=%d)\n", err_sig[i], result, errno);
		tp_assert(result == -1, "ERROR:sigaction return success.");
		tp_assert(errno == EINVAL, "ERROR:sigaction return other error number.");
	}

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

