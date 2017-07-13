/* entry-pth.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREAD_NUM 512

int thread_num;
char *filepath;
pthread_barrier_t barrier;

void read_write(int num, const char *path) {
	FILE* fp;
	char buf[256];
	size_t cnt;
	int i;

	if ((fp=fopen(path,"r")) == NULL) {
		perror("fopen failed: ");
		return;
	}

	buf[0] = 'A' + num % ('Z' - 'A' + 1);
	buf[1] = ':';
	cnt = fread(buf+2, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0])-2, fp);
	buf[cnt+2] = '\0';

	for (i = 0; i < num; i++) {
		pthread_barrier_wait(&barrier);
	}

	printf("%d: %s\n", num, buf);

	for (; i < thread_num; i++) {
		pthread_barrier_wait(&barrier);
	}

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
	int i = 0, ret = 0;
	int tn[MAX_THREAD_NUM];
	int ncpu;
	struct rlimit rlimit_nproc;

	if (argc != 4) {
		printf("invalid argment. %s ostype-filepath <thread_num> <rlimit_nproc>\n", argv[0]);
		return 0;
	}

	filepath = argv[1];

	thread_num = atoi(argv[2]);

	if ((thread_num < 1) || (MAX_THREAD_NUM < thread_num)) {
		printf("thread_number invalid, 1 <= thread_num <= %d\n", MAX_THREAD_NUM);
		return 0;
	}
	printf("use thread_num = %d.\n", thread_num);

	ncpu = atoi(argv[3]);
	getrlimit(RLIMIT_NPROC, &rlimit_nproc);
	if (ncpu <= 0 || ncpu > rlimit_nproc.rlim_max) {
		printf("rlimit_nproc invalid, 1 <= rlimit_nproc <= rlim_max\n");
		return 0;
	}
	rlimit_nproc.rlim_cur = ncpu;
	ret = setrlimit(RLIMIT_NPROC, &rlimit_nproc);
	if (ret != 0) {
		printf("setrlimit error\n");
		return 0;
	}
	
	for (i = 0; i < thread_num; i++) {
		tn[i] = i;
	}

	ret = pthread_barrier_init(&barrier, NULL, thread_num);
	if (ret != 0) {
		printf("pthread_barrier_init error\n");
		return 0;
	}

	for(i = 1; i < thread_num; i++) {
		// Call the clone system call to create the child thread 
		ret = pthread_create(&child[i], NULL, threadFunction, (void *)&tn[i]);
		if(ret) {
			printf("Failed to create thread[%d]. ret = %d\n", i, ret);
			return 0;
		}
	}

	read_write(0, filepath);

	for(i = 1; i < thread_num; i++) {
		ret = pthread_join(child[i], NULL);
		if(ret) {
			printf("Failed to join thread[%d]. ret = %d\n", i, ret);
			return 0;
		}
	}

	return 0; 
}
