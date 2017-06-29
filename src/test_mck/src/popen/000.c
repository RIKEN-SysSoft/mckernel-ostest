/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

#define BUF_LEN 16

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	FILE *fpread, *fpwrite;
	char buf[BUF_LEN];
	char *read_cmd = "echo Hello World";
	char *write_cmd = "cat";
	int error = 0;

	if ((fpread = popen(read_cmd, "r")) == NULL) {
		printf("read popen()=%p, errno=%d\n", fpread, errno);
		error = 1;
		goto err;
	}

	if ((fpwrite = popen(write_cmd, "w")) == NULL) {
		printf("write popen()=%p, errno=%d\n", fpwrite, errno);
		error = 1;
		goto rclose_err;
	}

	while (fgets(buf, BUF_LEN, fpread) != NULL) {
		if ((fputs(buf, fpwrite)) == -1) {
			printf("fputs()=-1, errno=%d\n", errno);
			error = 1;
			break;
		}
	}

	if ((pclose(fpwrite)) == -1) {
		printf("write pclose()=-1, errno=%d\n", errno);
		error = 1;
	}

rclose_err:
	if ((pclose(fpread)) == -1) {
		printf("read pclose()=-1, errno=%d\n", errno);
		error = 1;
	}

err:
	/* error check. */
	tp_assert(error == 0, "TP failed.");
	return NULL;
}
