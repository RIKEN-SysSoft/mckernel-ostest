/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <errno.h>
#include <sys/uio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

//#define D(fmt, ...) printf("[%d]" fmt, TEST_NUMBER, ##__VA_ARGS__)
#define D(fmt, ...) do{}while(0)

#define CHILD_PROCESS_TP_NUM 999

ssize_t call_process_vm_writev(pid_t pid,
			      const struct iovec *lvec, unsigned long liovcnt,
			      const struct iovec *rvec, unsigned long riovcnt,
			      unsigned long flags);

#endif /*__TEST_SUITE_H__*/
