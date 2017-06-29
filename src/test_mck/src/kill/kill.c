/* kill.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

void* kill_parse_args(int argc, char** argv)
{
	static struct kill_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			args.proc_num = atoi(optarg);
			break;
		default:
			break;
		}
	}

	return &args;
}

void kill_sig_handler(int sig)
{
	pid_t pid = getpid();
	fprintf(stdout, "[pid:%d, pgid:%d] Get signal.\n", pid, getpgid(pid));
	sig_count = 1;
}

void kill_child_fn(void)
{
	if(signal(SIGUSR1, &kill_sig_handler) == SIG_ERR){
		fprintf(stderr, "signal(SIGUSR1, ..) failed");
		_exit(1);
	}

	while(sig_count == 0) {
		cpu_pause();
	}
	_exit(0);
}
