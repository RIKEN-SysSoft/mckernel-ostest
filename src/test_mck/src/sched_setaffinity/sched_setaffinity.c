/* sched_setaffinity.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

void* sched_setaffinity_parse_args(int argc, char** argv)
{
	static struct sched_setaffinity_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			args.num_of_cpus = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return &args;
}
