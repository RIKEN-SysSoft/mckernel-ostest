/* 010.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0);

static int sig_count_parent = 0;
static int sig_count_child1 = 0;
static int sig_count_child2 = 0;

static void child_sig_hadl_p(int sig)
{
	sig_count_parent++;
}
static void child_sig_hadl_c1(int sig)
{
	sig_count_child1++;
}
static void child_sig_hadl_c2(int sig)
{
	sig_count_child2++;
}


static void child1_fn(int notify_fd)
{
	float f0 = 3.14;
	float f1 = 42.195;
	float f2 = 53.85;
	float f3 = 909.909;

	unsigned long xg[] = { 0x00000000ffffffff, 0xffffffff00000000, 0x0000ffff0000ffff, 0xffff0000ffff0000 };
	float xf[] = { 11.11, 22.22, 33.33, 44.44 };

	pid_t cur_pid = getpid();

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = child_sig_hadl_c1;
	act.sa_flags = SA_RESETHAND;
	sigaction(SIGUSR1, &act, NULL);

	/*
	 * 各種レジスタに値を設定
	 */
	write_xg(xg[0], xg[1], xg[2], xg[3]);
	write_xf(&xf[0], &xf[1], &xf[2], &xf[3]);

	printf("==[child1:%d on cpu%d] before migrate==\n", cur_pid, get_cpu_id());
	printf("[pid:%d] f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", cur_pid, f0, f1, f2, f3);
	show_extended_registers();

	// notify current cpuid
	{
		int cur_cpu = get_cpu_id();
		write(notify_fd, &cur_cpu, sizeof(int));
	}

	// Wait for send signal by parent process.
	while(sig_count_child1 == 0) {
		cpu_pause();
	}

	/*
	 * 設定した値が復元されていることを確認
	 */
	printf("==[child1:%d on cpu%d] after migrate==\n", cur_pid, get_cpu_id());
	printf("[pid:%d] f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", cur_pid, f0, f1, f2, f3);
	show_extended_registers();
}

static void child2_fn(void)
{
	float f0 = 6.28;
	float f1 = 84.391;
	float f2 = 106.161;
	float f3 = 99.9999;

	unsigned long xg[] = { 0x1111111111111111, 0xffffffffffffffff, 0xaaaaaaaaaaaaaaaa, 0x5555555555555555 };
	float xf[] = { 98.76, 54.32, 12.34, 56.78 };

	pid_t cur_pid = getpid();

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = child_sig_hadl_c2;
	act.sa_flags = SA_RESETHAND;
	sigaction(SIGUSR1, &act, NULL);

	/*
	 * 各種レジスタに値を設定
	 */
	write_xg(xg[0], xg[1], xg[2], xg[3]);
	write_xf(&xf[0], &xf[1], &xf[2], &xf[3]);

	printf("==[child2:%d on cpu%d] before migrate==\n", cur_pid, get_cpu_id());
	printf("[pid:%d] f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", cur_pid, f0, f1, f2, f3);
	show_extended_registers();

	// wakeup parent process.
	kill(getppid(), SIGUSR2);

	// Wait for send signal by parent process.
	while(sig_count_child2 == 0) {
		cpu_pause();
	}

	/*
	 * 設定した値が復元されていることを確認
	 */
	printf("==[child2:%d on cpu%d] after migrate==\n", cur_pid, get_cpu_id());
	printf("[pid:%d] f0 = %f, f1 = %f, f2 = %f, f3 = %f\n", cur_pid, f0, f1, f2, f3);
	show_extended_registers();
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;

	int result = -1;
	int child_pid1, child_pid2;
	int cpuid_child1 = 0;
	int pipe_fd[2];

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = child_sig_hadl_p;
	sigaction(SIGUSR2, &act, NULL);

	/* create pipe */
	result = pipe(pipe_fd);
	tp_assert(result != -1, "make pipe failed");

	child_pid1 = fork();

	if(child_pid1 == -1){
		/* error */
		tp_assert(0, "child 1 fork() Failed." );
	} else if (child_pid1 == 0) {
		/* child process 1 */
		child1_fn(pipe_fd[1]);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		_exit(0);

	} else {

		// Wait for notify cpuid by child process 1.
		read(pipe_fd[0], &cpuid_child1, sizeof(int));
		close(pipe_fd[0]);
		close(pipe_fd[1]);

		// gen next child proc.
		child_pid2 = fork();
		if(child_pid2 == -1) {
			// release child1
			kill(child_pid1, SIGUSR1);
			tp_assert(0, "child 2 fork() Failed." );
		} else if (child_pid2 == 0) {
			/* child process 2 */
			child2_fn();
			_exit(0);
		} else {
			/* parent process */
			int status = 1;
			cpu_set_t *cpusetp_child;
			size_t size;
			int num_cpus;
	
			result = setpgid(child_pid1, child_pid1);
			if(result == -1){
				kill(child_pid1, SIGKILL);
				kill(child_pid2, SIGKILL);
				tp_assert(0, "[parent] setpgid to child1 faild.");
			}
			result = setpgid(child_pid2, child_pid1);
			if(result == -1){
				kill(child_pid1, SIGKILL);
				kill(child_pid2, SIGKILL);
				tp_assert(0, "[parent] setpgid child2 faild.");
			}
	
			num_cpus = args->num_of_cpus;
			size = CPU_ALLOC_SIZE(num_cpus);
	
			// set child process affinity
			cpusetp_child = CPU_ALLOC(num_cpus);
			tp_assert(cpusetp_child != NULL, "(child process) alloc failed.");
	
			CPU_ZERO_S(size, cpusetp_child);
			CPU_SET_S(cpuid_child1, size, cpusetp_child);
	
			// Wait for send signal by child2 process.
			while(sig_count_parent == 0) {
				cpu_pause();
			}
			sig_count_parent = 0; // after wait, clear flag.
	
			// set child2 affinity equals to child1
			result = sched_setaffinity(child_pid2, size, cpusetp_child);
			CPU_FREE(cpusetp_child);
			if(result == -1){
				kill(-child_pid2, SIGUSR1);
				tp_assert(result != -1, "sched_setaffinity failed from to process.");
			}
	
			// wakeup childlen.
			kill(child_pid2, SIGUSR1);
			kill(child_pid1, SIGUSR1);

			waitpid(child_pid1, &status, 0);
			waitpid(child_pid2, &status, 0);
			printf("==Please check migrated child process keep datas==\n");
	
			tp_assert(result != -1, "sched_setaffinity failed to child process.");
		}
	}

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
