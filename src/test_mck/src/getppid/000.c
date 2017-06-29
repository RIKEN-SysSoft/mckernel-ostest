/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"

#define PIPE_BUF_SIZE 0x10

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static char* getppid0_parent_fn(pid_t child_pid, int read_fd, int write_fd)
{
	char buf[PIPE_BUF_SIZE];
	pid_t p_pid;

	/* wait for sync */
	read(read_fd, buf, PIPE_BUF_SIZE);

	/* get parent pid */
	p_pid = getpid();
	printf("[parent] pid=%d\n", p_pid);

	/* ppid write */
	write(write_fd, &p_pid, sizeof(p_pid));

	/* wait for sync */
	read(read_fd, buf, PIPE_BUF_SIZE);
	return NULL;
}

static char* getppid0_child_fn(int read_fd, int write_fd)
{
	pid_t c_ppid, p_pid;

	/* get parent pid */
	c_ppid = getppid();
	printf("[child] ppid=%d\n", c_ppid);

	/* sync */
	write(write_fd, "1234567890", PIPE_BUF_SIZE+1);

	/* ppid read */
	read(read_fd, &p_pid, sizeof(p_pid));

	/* cmp parent.getpid(), child.getppid() */
	tp_assert(c_ppid == p_pid, "[child] getppid faild.");

	/* sync */
	write(write_fd, "1234567890", PIPE_BUF_SIZE+1);
	return NULL;
}

static void close_all_pipes(int *fdi, int *fdj)
{
	close(fdi[0]);
	close(fdi[1]);
	close(fdj[0]);
	close(fdj[1]);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t child_pid;
	int ret;
	int p2c[2];
	int c2p[2];

	/* Create a pipe for synchronization */
	ret = pipe(p2c);
	tp_assert(ret != -1, "make pipe failed");
	ret = pipe(c2p);
	if(ret == -1){
		close(p2c[0]);
		close(p2c[1]);
	}
	tp_assert(ret != -1, "make pipe failed");

	/* Create child process. */
	child_pid = fork();
	tp_assert(child_pid != -1, "fork faild.");

	if(child_pid == 0) {
		getppid0_child_fn(p2c[0], c2p[1]);
		close_all_pipes(p2c, c2p);
		_exit(0);
	} else {
		getppid0_parent_fn(child_pid, c2p[0], p2c[1]);
		close_all_pipes(p2c, c2p);
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
