/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd;
	sigset_t ss;
	int result;

	// Open signalfd
	sigemptyset(&ss);
	fd = signalfd4(-1, &ss, SFD_NONBLOCK);
	tp_assert(fd != -1, "signalfd4() failed.");

	// check fd was closed
	result = fcntl(fd, F_GETFD);
	tp_assert(result != -1, "fcntl() failed.");
	tp_assert((result & O_NONBLOCK) == 0, "signalfd() does not set non-blocking mode.");

	// close signalfd
	result = close(fd);
	tp_assert(result == 0, "close(fd) failed.");

	// check fd was closed
	result = fcntl(fd, F_GETFD);
	tp_assert(result == -1, "why fcntl() successed?");
	tp_assert(errno == EBADF, "file descriptor not closed!!");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
