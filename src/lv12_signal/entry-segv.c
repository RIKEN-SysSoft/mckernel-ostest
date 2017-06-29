/* entry-segv.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h>
#include <stdio.h>
#include <string.h>

void sig_handler(int sig_num)
{
	printf("sig_num = %d received.\n", sig_num);
	if (signal(sig_num, SIG_DFL) == SIG_ERR) {
		printf("signal function default failed. sig_num = %d\n", sig_num);
		return;
	}
	return;
}

int main(int argc, char** argv)
{
	char buf[] = "aabbcc";
	int sig_num = SIGSEGV;

	if (signal(sig_num, sig_handler) == SIG_ERR) {
		printf("signal function set failed. sig_num = %d\n", sig_num);
		return 0;
	}
	memcpy((void *)-1, buf, sizeof(buf));
	printf("main end.\n");
}
