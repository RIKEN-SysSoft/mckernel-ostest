/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <string.h>
#include "arch_test_fpregs.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fpregs fregs0, fregs1;
	int result;

	memcpy(&fregs0, FPREGS_TEST_DATA, sizeof(struct fpregs));
	memset(&fregs1, 0, sizeof(struct fpregs));

	// アクセス可否、RW確認
	show_current_fpregs();
	fpregs_write(&fregs0);
	// show_current_fpregs();
	fpregs_read(&fregs1);

	result = memcmp(&fregs0, &fregs1, sizeof(struct fpregs));
	tp_assert(result == 0, "fpregs access error.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
