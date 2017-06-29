/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd;
	sigset_t ss;
	int result;

	// Open signalfd
	sigemptyset(&ss);
	fd = signalfd4(-1, &ss, 0);
	tp_assert(fd != -1, "signalfd4() failed.");

	// close signalfd
	result = close(fd);
	tp_assert(result == 0, "close(fd) failed.");

	// check fd was closed
	result = fcntl(fd, F_GETFD);
	tp_assert(result == -1, "why fcntl() successed?");
	tp_assert(errno == EBADF, "file descriptor not closed!!");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
