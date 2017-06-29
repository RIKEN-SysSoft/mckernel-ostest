/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	sigset_t set;

	// 各タイマに応じて発動するシグナルを無効化
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);   // ITIMER_REAL
	sigaddset(&set, SIGVTALRM); // ITIMER_VIRTUAL
	sigaddset(&set, SIGPROF);   // ITIMER_PROF
	sigprocmask(SIG_BLOCK, &set, NULL);

	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int result;
	const time_t init_time = 10; /* sec. */

	int itimer_type[] = {
		ITIMER_REAL,
		ITIMER_VIRTUAL,
		ITIMER_PROF,
	};
	char* itimer_name[] = {
		"ITIMER_REAL",
		"ITIMER_VIRTUAL",
		"ITIMER_PROF",
	};

	for (i = 0; i < sizeof(itimer_type)/sizeof(itimer_type[0]); ++i)
	{
		int cnt;
		struct itimerval old_value = {{0},{0}};
		struct itimerval new_value = {
			{        0, 0}, /* it_interval */
			{init_time, 0}  /* it_value */
		};

		// printf("%s set interval => %d\n", itimer_name[i], init_time);
		result = setitimer(itimer_type[i], &new_value, NULL);
		tp_assert(result == 0, "setitimer(1st) failed.");

		printf("testing %s, please wait...\n", itimer_name[i]);
		for(cnt = 0; cnt < INT_MAX; ++cnt){
			// spent a cpu time.
		};

		/* stop interval timer */
		new_value.it_value.tv_sec =  0;
		new_value.it_value.tv_usec = 0;

		result = setitimer(itimer_type[i], &new_value, &old_value);
		tp_assert(result == 0, "setitimer(2nd) failed.");

		// 起動したタイマカウンタは減っているはず
		// printf("%s remain => %d\n", itimer_name[i], old_value.it_value.tv_sec);
		tp_assert(old_value.it_value.tv_sec <= init_time, 
			"Why timer counter not reduced?");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
