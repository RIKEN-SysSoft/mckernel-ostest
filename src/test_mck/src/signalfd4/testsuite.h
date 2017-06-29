/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/signalfd.h>

int signalfd4(int fd, const sigset_t *mask, int flags);

#ifndef SIGSETSIZE
#define SIGSETSIZE 8
#endif

#endif /*__TEST_SUITE_H__*/
