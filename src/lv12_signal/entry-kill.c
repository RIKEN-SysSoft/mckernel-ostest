/* entry-kill.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <stdlib.h>
#include "arch_sigtable.h"

void sig_handler(int sig_num)
{
	printf("sig_num = %d, sig_name:%s received.\n", sig_num, sig_name[sig_num]);
	if (signal(sig_num, SIG_DFL) == SIG_ERR) {
		printf("signal function default failed. sig_num = %d, sig_name:%s\n", sig_num, sig_name[sig_num]);
		return;
	}
	return;
}

int main(int argc, char** argv) {
	int i = 0;

	for (i = 0; i < sizeof(sig_num) / sizeof(sig_num[0]); i++) {
		if (signal(sig_num[i], sig_handler) == SIG_ERR) {
			printf("signal function set failed. sig_num = %d, sig_name:%s\n", sig_num[i], sig_name[sig_num[i]]);
			continue;
		}
		if (kill(getpid(), sig_num[i])) {
			printf("kill faild. sig_num = %d, sig_name:%s\n", sig_num[i], sig_name[sig_num[i]]);
			return 0;
		}
	}

	printf("end of user program.\n");
	return 0; 
}
