/* entry-st.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <signal.h> 
#include <sched.h> 
#include <stdio.h>

unsigned char stack[1024*64];

void read_write(int num, const char *path) {
	FILE* fp;
	char buf[256];
	size_t cnt;

	if ((fp=fopen(path,"r")) == NULL) {
		perror("fopen failed: ");
		return;
	}

	buf[0] = '0' + num;
	buf[1] = ':';
	cnt = fread(buf+2, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0])-2, fp);
	buf[cnt+2] = '\0';

	printf("%s\n", buf);
	fclose(fp);
}


int main(int argc, char** argv) {
	char *path = NULL;

	if (argc != 2) {
		printf("invalid argment. %s <ostype filepath>\n", argv[0]);
		return 0;
	}
	path = argv[1];
	read_write(0, path);
	return 0; 
}
