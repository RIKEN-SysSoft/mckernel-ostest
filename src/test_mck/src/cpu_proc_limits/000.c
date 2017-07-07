/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "test_mck.h"
#include "testsuite.h"

#define PIPE_BUF_SIZE 0x10

#define PIDX_READ  0
#define PIDX_WRITE 1

#define CHILD_SLEEP_SEC	3

typedef struct {
	int p2c[2];
	int c2p[2];
} proc_pipes_t;

static void child_fn(int id, int read_fd, int write_fd)
{
	char buf[PIPE_BUF_SIZE];

	/* wait until read success... */
	if (read(read_fd, buf, PIPE_BUF_SIZE) <= 0) {
		return;
	}
	// fprintf(stdout, "[child]pid=%d, fork_id=%d, buf=%s\n", getpid(), id, buf);

	sleep(CHILD_SLEEP_SEC);
	memset(buf, 0, sizeof(buf));

	/* write back to parent. */
	snprintf(buf, sizeof(buf), "%010d", getpid());
	write(write_fd, buf, sizeof(buf));
}

//void close_pipes(int *fd)
//{
//	close(fd[0]);
//	close(fd[1]);
//}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct cpu_proc_limits_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(tc_argc, tc_argv, "p:c:")) != -1) {
		switch (opt) {
		case 'p':
			args.proc_num = atoi(optarg);
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

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int pid1;
	int forks = 0;
	char buf[PIPE_BUF_SIZE];
	struct cpu_proc_limits_args *args = (struct cpu_proc_limits_args*)tc_arg;
	proc_pipes_t *pp = NULL;
	struct rlimit rlimit_nproc;
	int ret;

	tp_assert(args != NULL, "internal error.");
	tp_assert(0 < args->proc_num, "-p <child proc num> invalid argument.");
	tp_assert(0 < args->cpu_num, "-c <num cpus> invalid argument.");

	getrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(rlimit_nproc.rlim_max >= args->cpu_num, "-c <num cpus> is larger than hard limit");
	rlimit_nproc.rlim_cur = args->cpu_num;
	ret = setrlimit(RLIMIT_NPROC, &rlimit_nproc);
	tp_assert(ret == 0, "setrlimit error");

	pp = calloc(args->proc_num, sizeof(proc_pipes_t));
	tp_assert(pp != NULL, "calloc error.");

forkone:
	/* Create a pipe for synchronization */
	{
		int p2c_ret = 0, c2p_ret = 0;

		p2c_ret = pipe(pp[forks].p2c);
		c2p_ret = pipe(pp[forks].c2p);

		if(p2c_ret == -1 || c2p_ret == -1){
			for(i=0; i<forks; i++){
				close(pp[i].p2c[1]);
				close(pp[i].c2p[0]);
			}
			if (p2c_ret != -1) {
				close(pp[i].p2c[0]);
				close(pp[i].p2c[1]);
			}
			if (c2p_ret != -1) {
				close(pp[i].c2p[0]);
				close(pp[i].c2p[1]);
			}
			free(pp);
			tp_assert(NULL, "pipe open faild!!\n");
		}
	}

	/* Create child process. */
	pid1 = fork();

	if(pid1 != 0){
		/* parent */
		++forks;
		if ((pid1 != (-1)) && (forks < args->proc_num)) {
			close(pp[forks - 1].p2c[0]);
			close(pp[forks - 1].c2p[1]);
			goto forkone;
		}
		else if (pid1 < 0) {
			for (i = 0; i < forks; i++) {
				close(pp[i].p2c[1]);
				close(pp[i].c2p[0]);
			}
			free(pp);
			tp_assert(NULL, "fork() Failed." );
		}
	} else {
		/* child */

		close(pp[forks].p2c[1]);
		close(pp[forks].c2p[0]);

		child_fn(forks, pp[forks].p2c[PIDX_READ], pp[forks].c2p[PIDX_WRITE]);

		close(pp[forks].p2c[0]);
		close(pp[forks].c2p[1]);

		free(pp);
		_exit(0);
	}

	// printf("Kick children process.\n");
	// printf("When fork_id is overlap, then error.\n");
	snprintf(buf, sizeof(buf), "%010d", getpid());
	for(i=0; i<args->proc_num; i++){
		write(pp[i].p2c[PIDX_WRITE], "1234567890", PIPE_BUF_SIZE+1);
	}

	printf("Wait children's process will be finished. (about %dsec...)\n", CHILD_SLEEP_SEC);
	for(i=0; i<args->proc_num; i++){
		memset(buf, 0, sizeof(buf));

		read(pp[i].c2p[PIDX_READ], buf, PIPE_BUF_SIZE);
		// printf("[parent]child process(pid=%s) is finished.\n", buf);

		close(pp[i].p2c[1]);
		close(pp[i].c2p[0]);
	}
	free(pp);

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
