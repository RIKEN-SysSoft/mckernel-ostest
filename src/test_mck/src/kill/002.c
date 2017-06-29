/* 002.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

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

	pid_t child_pgid;

	int fork_error_flag = 0;

	tp_assert(args->proc_num > 0, "mandatory parameter '-p <num_of_child_procs>'");

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

	child_pgid = child_pids[0];	// set first pid to new pgid.

	printf("wait for a few sec...\n");
	sleep(3);

	for(cnt = 0 ; cnt < args->proc_num - fork_error_flag; cnt++){
		printf("Set child process(pid:%d) pgid:%d => %d\n", child_pids[cnt], getpgid(child_pids[cnt]), child_pgid);
		if(setpgid(child_pids[cnt], child_pgid) != 0){
			printf("<WARNING> Failed to set the PGID to child process %d.\n", child_pids[cnt]);
		}
	}

	printf("send signal to target pgid(=%d)s\n", child_pgid);
	kill(-child_pgid, SIGUSR1);

	// Parent process wants most recently ended.
	for(cnt = 0; cnt < args->proc_num; cnt++){
		status = 1;
		waitpid(child_pids[cnt], &status, 0);
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
