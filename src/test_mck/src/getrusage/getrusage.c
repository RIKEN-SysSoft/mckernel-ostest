/* getrusage.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "testsuite.h"

void show_rusage(struct rusage *usage)
{
	if (!usage) {
		printf("show_rusage(): usage is NULL");
		return;
	}
	printf("------------------------------\n");
	printf("show_rusage():\n");
	printf("  ru_utime=%lds + %ldus\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
	printf("  ru_stime=%lds + %ldus\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
	printf("  ru_maxrss=%ld\n", usage->ru_maxrss);

	if (usage->ru_ixrss) printf("  ru_ixrss=%ld\n", usage->ru_ixrss);
	if (usage->ru_idrss) printf("  ru_idrss=%ld\n", usage->ru_idrss);
	if (usage->ru_isrss) printf("  ru_isrss=%ld\n", usage->ru_isrss);
	if (usage->ru_minflt) printf("  ru_minflt=%ld\n", usage->ru_minflt);
	if (usage->ru_majflt) printf("  ru_majflt=%ld\n", usage->ru_majflt);
	if (usage->ru_nswap) printf("  ru_nswap=%ld\n", usage->ru_nswap);
	if (usage->ru_inblock) printf("  ru_inblock=%ld\n", usage->ru_inblock);
	if (usage->ru_oublock) printf("  ru_oublock=%ld\n", usage->ru_oublock);
	if (usage->ru_msgsnd) printf("  ru_msgsnd=%ld\n", usage->ru_msgsnd);
	if (usage->ru_msgrcv) printf("  ru_msgrcv=%ld\n", usage->ru_msgrcv);
	if (usage->ru_nsignals) printf("  ru_nsignals=%ld\n", usage->ru_nsignals);
	if (usage->ru_nvcsw) printf("  ru_nvcsw=%ld\n", usage->ru_nvcsw);
	if (usage->ru_nivcsw) printf("  ru_nivcsw=%ld\n", usage->ru_nivcsw);
	printf("------------------------------\n");
	return;
}

int send_and_show_rusage(int who)
{
	int ret = 0;
	struct rusage usage;

	memset(&usage, 0, sizeof(struct rusage));
	ret = getrusage(who, &usage);
	if (ret) {
		printf("getrusage() failed. (%d)\n", errno);
		return ret;
	}
	show_rusage(&usage);
	return ret;
}
