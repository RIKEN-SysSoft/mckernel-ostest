/* 006.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256
#define BUF_LEN PAGE_SIZE

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	FILE *fp = NULL;
	char pfname[PROCFILE_LEN] = "/proc/cpuinfo";
	char buf[BUF_LEN];

	/* fopen */
	if ((fp = fopen(pfname, "r")) == NULL) {
		printf("fopen() failed. %d\n", errno);
		goto err;
	}

	/* dump */
	printf("output: %s\n", pfname);
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		printf("%s", buf);
	}

	/* fclose */
	if (fclose(fp)) {
		printf("fclose() failed. %d\n", errno);
		goto err;
	}
	tp_assert(0, "you need check CPUINFO");

err:
	/* tp error */
	tp_assert(0, "TP failed.");
}
