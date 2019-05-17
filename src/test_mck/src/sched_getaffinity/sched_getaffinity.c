/* sched_getaffinity.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

void* sched_getaffinity_parse_args(int argc, char** argv)
{
	static struct sched_getaffinity_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(argc, argv, "f:p:o:")) != -1) {
		switch (opt) {
		case 'p':
			args.num_of_cpus = atoi(optarg);
			break;
		case 'f':
			args.file_path = optarg;
			break;
		case 'o':
			args.cpuset_path = optarg;
			break;
		default:
			break;
		}
	}
	args.argv = argv + optind;
	args.argc = argc - optind;

	return &args;
}
