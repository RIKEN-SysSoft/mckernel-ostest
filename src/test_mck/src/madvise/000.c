/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <stdio.h>

static const struct madvise000_param {
	int advise_flags;
	int expect_result;
	int expect_error;
} madvise000_params[] = {
	{ 0 /*MADV_DONTNEED*/,		TEST_MADV_SUCCESS, 	0},	/* 0 */
	{ 1 /*MADV_NORMAL*/,		TEST_MADV_SUCCESS, 	0},	/* 1 */
	{ 2 /*MADV_RANDOM*/,		TEST_MADV_SUCCESS, 	0},	/* 2 */
	{ 3 /*MADV_SEQUENTIAL*/,	TEST_MADV_SUCCESS, 	0},	/* 3 */
	{ 4 /*MADV_WILLNEED*/,		TEST_MADV_SUCCESS, 	0},	/* 4 */
	{ 9 /*MADV_REMOVE*/,		TEST_MADV_FAILUER, 	EACCES},/* 5 */
	{ 10 /*MADV_DONTFORK*/,		TEST_MADV_SUCCESS, 	0},	/* 6 */
	{ 11 /*MADV_DOFORK*/,		TEST_MADV_SUCCESS, 	0},	/* 7 */
	{ 12 /*MADV_DODUMP*/,		TEST_MADV_FAILUER, 	EINVAL},/* 8 */
	{ 13 /*MADV_DONTDUMP*/,		TEST_MADV_FAILUER, 	EINVAL},/* 9 */
	{ 14 /*MADV_MERGEABLE*/,	TEST_MADV_FAILUER, 	EINVAL},/* 10 */
	{ 15 /*MADV_UNMERGEABLE*/,	TEST_MADV_FAILUER, 	EINVAL},/* 11 */
	{ 16 /*MADV_HUGEPAGE*/,		TEST_MADV_FAILUER, 	EINVAL},/* 12 */
	{ 17 /*MADV_NOHUGEPAGE*/,	TEST_MADV_FAILUER, 	EINVAL},/* 13 */
	{ 100 /*MADV_HWPOISON*/,	TEST_MADV_FAILUER, 	EPERM},	/* 14 */
	{ 101 /*MADV_SOFT_OFFLINE*/,	TEST_MADV_FAILUER, 	EPERM},	/* 15 */
};

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct madvise_args* args;
	args = madvise_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct madvise_args* args;
	const struct madvise000_param* param = NULL;

	args = tc_arg;
	tp_assert(args->file != NULL, "invalid -f option.");
	tp_assert(0 <= tc_num, "invalid tp number.");
	tp_assert(tc_num < sizeof(madvise000_params)/sizeof(madvise000_params[0]),
			  "invalid tp number.");

	param = &madvise000_params[tc_num];
	return madvise_simple(args->file, param->advise_flags,
					   param->expect_result, param->expect_error);
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
