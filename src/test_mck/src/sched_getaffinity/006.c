/* 006.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

static void child_execve(struct sched_getaffinity_args* args)
{
	char **argv;
	extern char **environ;
	int i = 0;

	argv = malloc(args->argc * (sizeof(char **) + 2));
	argv[0] = args->file_path;
	for (i = 0; i < args->argc; i++) {
		argv[i + 1] = *(args->argv + i);
	}
	argv[i + 1] = NULL;

	execve(args->file_path, argv, environ);
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return sched_getaffinity_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_getaffinity_args *args = (struct sched_getaffinity_args*)tc_arg;
	int result = -1;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus, cpu;
	int child_pid;
	int status = 1;

	tp_assert(args->file_path != NULL, "mandatory parameter '-p <num_of_cpus> -f <show_affinity_app_path>'");
	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus> -f <show_affinity_app_path>'");

	child_pid = fork();

	if(child_pid == -1){
		/* error */
		tp_assert(0, "fork() Failed." );
	} else if (child_pid == 0) {
		/* child process */
		num_cpus = args->num_of_cpus;
		cpusetp = CPU_ALLOC(num_cpus);
		tp_assert(cpusetp != NULL, "alloc failed.");

		size = CPU_ALLOC_SIZE(num_cpus);
		CPU_ZERO_S(size, cpusetp);

		// get own affinity
		result = sched_getaffinity(0, size, cpusetp);
		if(result == -1){
			CPU_FREE(cpusetp);
			tp_assert(0, "sched_getaffinity failed.");
		}

		printf("========CHILD  PROCESS========\n");
		for(cpu = 0; cpu < num_cpus; cpu++){
		        printf("CPU_ISSET(%d)=%s\n", cpu, CPU_ISSET(cpu, cpusetp)?"TRUE":"FALSE");         
		}       
		printf("========CHILD  PROCESS========\n");

		CPU_FREE(cpusetp);

		// do execve.
		child_execve(args);
		tp_assert(0, "Why return from execve() ???");

	} else {
		/* parent process */
		printf("==waiting for child process finish..==\n");
		result = waitpid(child_pid, &status, 0);
		printf("==Please check affinity is taken over after execve==\n");
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
