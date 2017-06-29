/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;

	int itimer_type[] = {
		ITIMER_REAL,
		ITIMER_VIRTUAL,
		ITIMER_PROF,
	};
	// char* itimer_name[] = {
	// 	"ITIMER_REAL",
	// 	"ITIMER_VIRTUAL",
	// 	"ITIMER_PROF",
	// };

	for (i = 0; i < sizeof(itimer_type)/sizeof(itimer_type[0]); ++i)
	{
		struct itimerval current = {
			{0xdeadbeef, 0xdeadbeef}, /* it_interval */
			{0xdeadbeef, 0xdeadbeef}  /* it_value */
		};

		int result = getitimer(itimer_type[i], &current);
		tp_assert(result == 0, "getitimer failed.");

		// タイマを起動していないので、取得できる値は０でなければおかしい。
		tp_assert(
			current.it_value.tv_sec == 0 && current.it_value.tv_usec == 0, 
			"why timer are non zero?"
		);
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
