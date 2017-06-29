/* 008.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, 0);

extern int sig_count;

static void child_sig_hadl(int sig)
{
	sig_count++;
}

static int get_migrate_target_cpuid(int parent, int child, int target)
{
	// printf("[DEBUG](%s)parent=%d, child=%d, target=%d\n", __func__, parent, child, target);
	if (child == target) {
		target = get_migrate_target_cpuid(parent, child, target - 1);
	}
	if ((target >= 0) && (parent == target)) {
		target = get_migrate_target_cpuid(parent, child, target - 1);
	}
	return target;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int child_pid;
	struct sigaction act;
	int ret = -1, c2p[2] = {-1, -1};

	tp_assert(3 <= args->num_of_cpus, "mandatory parameter '-p <num_of_cpus(3 or more)>'");

	memset(&act, 0, sizeof(act));
	act.sa_handler = child_sig_hadl;
	act.sa_flags = SA_RESETHAND;
	sigaction(SIGUSR2, &act, NULL);

	ret = pipe(c2p);
	tp_assert(ret == 0, "pipe() Failed.");

	child_pid = fork();

	if(child_pid == -1){
		/* error */
		tp_assert(0, "fork() Failed." );
	} else if (child_pid == 0) {
		/* child process */
		int cpuid_before_migreate, cpuid_after_migreate;

		struct sigaction act;
		memset(&act, 0, sizeof(act));
		act.sa_handler = child_sig_hadl;
		act.sa_flags = SA_RESETHAND;
		sigaction(SIGUSR1, &act, NULL);

		cpuid_before_migreate = get_cpu_id();
		printf("[child] before migrate cpuid=%d\n", cpuid_before_migreate);
		write(c2p[1], &cpuid_before_migreate, sizeof(cpuid_before_migreate));

		// wakeup parent process.
		kill(getppid(), SIGUSR2);

		// Wait for send signal by parent process.
		while(sig_count == 0) {
			cpu_pause();
		}

		cpuid_after_migreate = get_cpu_id();
		printf("[child] after migrate cpuid=%d\n", cpuid_after_migreate);
		write(c2p[1], &cpuid_after_migreate, sizeof(cpuid_after_migreate));

		_exit(0);

	} else {
		/* parent process */
		int status = 1;
		int result = -1;
		cpu_set_t *cpusetp_child;
		size_t size;
		int num_cpus;
		int c_cpuid_before_migreate, c_cpuid_after_migreate;
		int migrate_cpu = 0;
		int parent_cpu = get_cpu_id();

		// Wait for send signal by child process.
		while(sig_count == 0) {
			cpu_pause();
		}

		read(c2p[0], &c_cpuid_before_migreate, sizeof(c_cpuid_before_migreate));

		num_cpus = args->num_of_cpus;
		size = CPU_ALLOC_SIZE(num_cpus);

		// set child process affinity
		cpusetp_child = CPU_ALLOC(num_cpus);
		tp_assert(cpusetp_child != NULL, "(child process) alloc failed.");

		CPU_ZERO_S(size, cpusetp_child);

		// get migrate target cpuid
		/* printf("[DEBUG][parent] check cpuid: parent=%d, child=%d, target=%d\n", 
					parent_cpu, c_cpuid_before_migreate, num_cpus -1); */
		migrate_cpu = get_migrate_target_cpuid(
				parent_cpu, c_cpuid_before_migreate, num_cpus - 1);
		tp_assert(migrate_cpu >= 0, "Bad target to migrate child process.");
		/* printf("[DEBUG][parent] fix   cpuid: parent=%d, child=%d, target=%d\n", 
					parent_cpu, c_cpuid_before_migreate, migrate_cpu); */
		CPU_SET_S(migrate_cpu, size, cpusetp_child);

		result = sched_setaffinity(child_pid, size, cpusetp_child);
		CPU_FREE(cpusetp_child);

		// wakeup child process.
		kill(child_pid, SIGUSR1);

		read(c2p[0], &c_cpuid_after_migreate, sizeof(c_cpuid_after_migreate));

		waitpid(child_pid, &status, 0);

		close(c2p[0]);
		close(c2p[1]);

		tp_assert(c_cpuid_before_migreate != c_cpuid_after_migreate, "Really process running there?");
		tp_assert(result != -1, "sched_setaffinity failed to child affinity.");
	}

	/* テスト成功 */
	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
