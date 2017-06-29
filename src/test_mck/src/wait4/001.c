/* 001.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct wait4_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(tc_argc, tc_argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			args.filepath = optarg;
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	FILE *f;
	long pid_max = 0;
	pid_t err_pid = 0;

	int result = 0;
	int status = 1;
	struct rusage rusage;
	struct wait4_args *args = (struct wait4_args*)tc_arg;

	tp_assert(args != NULL, "-f <pid_max filepath>");
	tp_assert(args->filepath != NULL, "-f <pid_max filepath>");

	if((f = fopen(args->filepath, "r")) == NULL) {
		perror("fopen faild: ");
		tp_assert(0!=1, "invalid argment.");
	}

	fscanf(f, "%ld", &pid_max);
	err_pid = pid_max + 1;
	fclose(f);
	f = NULL;

	result = wait4(err_pid, &status, 0, &rusage);
	printf("wait4()=%d, errno=%d\n", result, errno);
	tp_assert(result == -1, "Why process(MAX_PID+1) is exist!?");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
