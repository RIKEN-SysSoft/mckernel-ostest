/* get_cpu_id.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/syscall.h>
#include <errno.h>

// If run on Linux, get_cpu_id will return ENOSYS.
int get_cpu_id(void)
{
	int id = (int)syscall(__NR_get_cpu_id);
	if(id < 0){
		printf("You ran the test in Linux? (errno=%d)\n", errno);
	}
	return id;
}
