/* getcpu.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include "test_mck.h"
#include "testsuite.h"

int getcpu(unsigned *cpu, unsigned *node)
{
	int ret = 0;
	ret = syscall(__NR_getcpu, cpu, node, NULL);
	return ret;
}

