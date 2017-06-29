/* show_affinity.c COPYRIGHT FUJITSU LIMITED 2015 */
#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
	int opt;
	int nr_cpus = 0, cpu;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			nr_cpus = atoi(optarg);
			break;
		default:
			break;
		}
	}

	if(nr_cpus == 0) {
		printf("nr_cpus is 0\n", nr_cpus);
		return -1;
	}

	cpu_set_t *set = CPU_ALLOC(nr_cpus);
	size_t setsize = CPU_ALLOC_SIZE(nr_cpus);
	sched_getaffinity(0, setsize, set);

	printf("========EXECED PROCESS========\n");
	for(cpu = 0; cpu < nr_cpus; cpu++){
		printf("CPU_ISSET(%d)=%s\n", cpu, CPU_ISSET(cpu, set)?"TRUE":"FALSE");
	}
	printf("========EXECED PROCESS========\n");

	CPU_FREE(set);
	return 0;
}

