/* 001.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/syscall.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return kill_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int cnt;
	pid_t *child_pids, child;
	struct kill_args *args = (struct kill_args*)tc_arg;
	int status = 1;

	int fork_error_flag = 0;

	tp_assert(args->proc_num > 0, "mandatory parameter '-p <num_of_child_procs>'");
	tp_assert(syscall(__NR_get_cpu_id) != -1,
		"Don't run this TP on HostLinux!!");

	child_pids = (int *)malloc(sizeof(int) * args->proc_num);

	for(cnt = 0; cnt < args->proc_num; cnt++)
	{
		child = fork();
		switch(child) {
			case -1:
				fork_error_flag++;
				fprintf(stderr, "[warning] fork() failed.\n");
				break;
			case 0:
				kill_child_fn();
				break;
			default:
				printf("Generate child process pid:%d, pgid:%d\n", child, getpgid(child));
				child_pids[cnt] = child;
				break;
		}

		if(fork_error_flag != 0) break;
	}

	if(cnt <= 0) {
		free(child_pids);
		tp_assert(0, "child process nothing.");
	}

	if(signal(SIGUSR1, &kill_sig_handler) == SIG_ERR){
		free(child_pids);
		fprintf(stderr, "signal(SIGUSR1, ..) failed (errno=%d)\n", errno);
		_exit(1);
	}

	printf("wait for a few sec...\n");
	sleep(3);

	printf("Send to all processes that have permission to send a signal.\n");
	kill(-1, SIGUSR1);

	// Parent process wants most recently ended.
	for(cnt = 0; cnt < args->proc_num; cnt++){
		status = 1;
		waitpid(child_pids[cnt], &status, 0);
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
