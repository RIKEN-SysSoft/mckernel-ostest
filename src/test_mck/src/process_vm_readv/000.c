/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#define BUF_SIZE  100

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid;
	char buf[BUF_SIZE];
	char addr[19];
	char* expr = "0";

	/* initialize memory area */
	memset(buf, expr[0], BUF_SIZE);
	snprintf(addr, sizeof(addr), "0x%lx", (unsigned long)buf);

	D("buf:%#p, addr:%s, size:%#x\n", buf, addr, BUF_SIZE);

	/* fork/execve */
	pid = fork();
	tp_assert(pid != -1, "fork error.");

	if (pid == 0) {
		char* argv[] = {
			the_app,
			"-s", STRING(TEST_SUITE),
			"-n", STRING(CHILD_PROCESS_TP_NUM),
			"--",
			"-a", addr,
			"-s", STRING(BUF_SIZE),
			"-e", expr,
			NULL,
		};
		/* child process */
		execve(the_app, argv, environ);
		/* execveは返らない */
		tp_assert(NULL, "execve error.");
	}
	/* parent process */
	{
		int result;
		int status;
		struct rusage rusage;
		result = wait4(pid, &status, 0, &rusage);
		tp_assert(result != -1, "wait4 error.");
		tp_assert(status == 0, "invalid exit code.");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
