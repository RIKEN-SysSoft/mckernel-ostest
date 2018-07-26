/* get_cpu_id.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/syscall.h>
#include <errno.h>
#include <sched.h>

// If run on Linux, get_cpu_id will return ENOSYS.
int get_cpu_id(void)
{
	int id = sched_getcpu();
	if (id < 0) {
		printf("Why sched_getcpu failed?\n");
	}
	return id;
}
