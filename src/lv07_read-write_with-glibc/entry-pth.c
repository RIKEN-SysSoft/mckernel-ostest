/* entry-pth.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <stdio.h>
#include <pthread.h>

#define MAX_THREAD_NUM 32

char *filepath;

void read_write(int num, const char *path) {
	FILE* fp;
	char buf[256];
	size_t cnt;
	if ((fp=fopen(path,"r")) == NULL) {
		perror("fopen faild: ");
		return;
	}

	buf[0] = '0' + num;	/* th[0]  = '0', th[1]  = '1', th[2]  = '2', th[3]  = '3', th[4]  = '4',
				   th[5]  = '5', th[6]  = '6', th[7]  = '7', th[8]  = '8', th[9]  = '9',
				   th[10] = ':', th[11] = ';', th[12] = '<', th[13] = '=', th[14] = '>',
				   th[15] = '?', th[16] = '@', th[17] = 'A', th[18] = 'B', th[19] = 'C',
				   th[20] = 'D', th[21] = 'E', th[22] = 'F', th[23] = 'G', th[24] = 'H',
				   th[25] = 'I', th[26] = 'J', th[27] = 'K', th[28] = 'L', th[29] = 'M',
				   th[30] = 'N', th[31] = 'O' */
	buf[1] = ':';
	cnt = fread(buf+2, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0])-2, fp);
	buf[cnt+2] = '\0';

	printf("%s\n", buf);
	fclose(fp);
}

// The child thread will execute this function 
void *threadFunction( void* argument ) {
	int *num = (int *)argument;
	read_write(*num, filepath);
	return 0; 
}

int main(int argc, char** argv) {
	pthread_t child[MAX_THREAD_NUM];
	int thread_num = 2;	/* default thread_num 2 */
	int i = 0, ret = 0;
	int tn[MAX_THREAD_NUM];

	if (argc == 1 || 3 < argc) {
		printf("invalid argment. %s ostype-filepath <thread_num[default=2]>\n", argv[0]);
		return 0;
	}

	filepath = argv[1];

	if (argc != 2) {
		thread_num = atoi(argv[2]);

		if ((thread_num < 1) || (MAX_THREAD_NUM < thread_num)) {
			printf("thread_number invalid, 1 <= thread_num <= %d\n", MAX_THREAD_NUM);
			return 0;
		}
		printf("use thread_num = %d.\n", thread_num);

	} else {
		printf("use default thread_num = 2.\n");
	}

	for (i = 0; i < MAX_THREAD_NUM; i++) {
		tn[i] = i;
	}

	for(i = 1; i < thread_num; i++) {
		// Call the clone system call to create the child thread 
		ret = pthread_create(&child[i], NULL, threadFunction, (void *)&tn[i]);
		if(ret) {
			printf("Faild to create thread[%d]. ret = %d\n", i, ret);
		}
	}

	read_write(0, filepath);

	for(i = 1; i < thread_num; i++) {
		ret = pthread_join(child[i], NULL);
		if(ret) {
			printf("Faild to join thread[%d]. ret = %d\n", i, ret);
		}
	}

	return 0; 
}
