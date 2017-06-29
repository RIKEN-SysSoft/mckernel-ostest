/* signalfd4.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

int signalfd4(int fd, const sigset_t *mask, int flags)
{
	return (int)syscall(__NR_signalfd4, fd, mask, SIGSETSIZE, flags);
}
