/* 003.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <errno.h>

#include <string.h>
#include "arch_test_fpregs.h"

/* get own path */
struct fpregs_args {
	char *file_path;
};

static struct fpregs_args args;
SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	args.file_path = tc_argv[0];
	return (void *)&args;
}

extern char** environ;
RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct fpregs fregs0;
	struct fpregs_args *args = tc_arg;

	char *execve_args[] = {
		"", "-s" ,"fpregs", "-n" ,"0", NULL
	};

	memcpy(&fregs0, FPREGS_TEST_DATA, sizeof(struct fpregs));
	fpregs_write(&fregs0);
	printf("before execve.\n");
	show_current_fpregs();

	printf("=== look at the contents of the register since run execve. ===\n");

	/* execveは返らない */
	execve(args->file_path, execve_args, environ);
	tp_assert(0==1, "Why return from execve() ???");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
