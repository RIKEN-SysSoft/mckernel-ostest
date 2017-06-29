/* 000.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"


#define TEST_PGID_OFFSET 2000
#define PIPE_BUF_SIZE 0x10

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static char* setpgid0_parent_fn(pid_t child_pid, int read_fd, int write_fd)
{
	char buf[PIPE_BUF_SIZE];
	int result = 0;

	// wait for sync
	read(read_fd, buf, PIPE_BUF_SIZE);

	printf("[parent] now rewrite child pgid\n");
	result = setpgid(child_pid, child_pid);
	tp_assert(result != -1, "[parent] setpgid faild.");

	write(write_fd, "1234567890", PIPE_BUF_SIZE+1);

	read(read_fd, buf, PIPE_BUF_SIZE);
	return NULL;
}

static char* setpgid0_child_fn(int read_fd, int write_fd)
{
	pid_t c_pid, c_pgid;
	char buf[PIPE_BUF_SIZE];

	c_pid = getpid();
	tp_assert(c_pid != -1, "[child] getpid faild.");
	c_pgid = getpgid(c_pid);
	tp_assert(c_pgid != -1, "[child] getpgid faild.");

	printf("[child] pid=%d, pgid=%d\n", c_pid, c_pgid);

	write(write_fd, "1234567890", PIPE_BUF_SIZE+1);
	read(read_fd, buf, PIPE_BUF_SIZE);

	c_pgid = getpgid(c_pid);
	tp_assert(c_pgid != -1, "[child] getpgid faild.");

	printf("[child] pid=%d, pgid=%d\n", c_pid, c_pgid);
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
		setpgid0_child_fn(p2c[0], c2p[1]);
		close_all_pipes(p2c, c2p);
		_exit(0);
	} else {
		setpgid0_parent_fn(child_pid, c2p[0], p2c[1]);
		close_all_pipes(p2c, c2p);
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
