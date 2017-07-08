/* cpu_thread_limits.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <sys/types.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>

#include "test_mck.h"
#include "testsuite.h"

struct cpu_thread_limits_args *cpu_thread_limits_get_param(int argc, char *argv[])
{
	static struct cpu_thread_limits_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(argc, argv, "t:c:")) != -1) {
		switch (opt) {
		case 't':
			args.thread_num = atoi(optarg);
			break;
		case 'c':
			args.cpu_num = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return &args;
}
