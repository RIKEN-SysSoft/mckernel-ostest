/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"
#include <string.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	struct clock_test_case {
		char* clock_name;
		int clock_id;
		int expect_result;
		int expect_errno;
	} ctc[] = {
		{"REALTIME", 		CLOCK_REALTIME		,  0,	0	},
		{"REALTIME_ALARM", 	CLOCK_REALTIME_ALARM	, -1,	EINVAL	},
		{"REALTIME_COARSE", 	CLOCK_REALTIME_COARSE	,  0,	0	},
		{"MONOTONIC", 		CLOCK_MONOTONIC		,  0,	0	},
		{"MONOTONIC_COARSE", 	CLOCK_MONOTONIC_COARSE	,  0,	0	},
		{"MONOTONIC_RAW", 	CLOCK_MONOTONIC_RAW	,  0,	0	},
		{"BOOTTIME", 		CLOCK_BOOTTIME		,  0,	0	},
		{"BOOTTIME_ALARM", 	CLOCK_BOOTTIME_ALARM	, -1,	EINVAL	},
		{"PROCESS_CPUTIME_ID", 	CLOCK_PROCESS_CPUTIME_ID,  0,	0	},
		{"THREAD_CPUTIME_ID", 	CLOCK_THREAD_CPUTIME_ID	,  0,	0	},
	};

	printf("[INFO] Show clocks' resolution.\n");
	for (i = 0; i < sizeof(ctc)/sizeof(ctc[0]); ++i)
	{
		struct timespec ts = {0};
		int result = clock_getres(ctc[i].clock_id, &ts);
		tp_assert(result == ctc[i].expect_result, "clock_getres return unexpected result.");
		if(result == 0) {
			printf("%-20s: %ld.%09ld\n", ctc[i].clock_name, ts.tv_sec, ts.tv_nsec);
		} else {
			printf("%-20s: %s %s\n", ctc[i].clock_name, strerror(errno),
				(errno == ctc[i].expect_errno) ? "(expected)":"(non expected)");
			tp_assert(errno == ctc[i].expect_errno, "clock_getres return unexpected errno.");
		}
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
