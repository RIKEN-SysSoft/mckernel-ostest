/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/types.h>

#define RT_SIGACTION_TEST_SIGNAL SIGUSR1

int set_rt_sigaction_hundler(void *handler, int flags, int sig_to_mask);

#endif /*__TEST_SUITE_H__*/
