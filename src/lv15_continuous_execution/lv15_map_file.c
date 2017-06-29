/* lv15_map_file.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#define TASK_UNMAPPED_BASE 0x0000007FE0000000UL

#define _4MB  (1024UL * 1024UL * 4UL)
#define _256MB  (1024UL * 1024UL * 256UL)

#define ALIGN_UP(a, b)  ( ((a)+(b)-1) & ~((b)-1) )

int main(int argc, char** argv)
{
	/*
	 * - file mapping
	 * - 4MB
	 * - ld
	 */
	size_t len = _4MB;
	char *addr;
	char *print;
	int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	int flag = MAP_PRIVATE | MAP_ANONYMOUS;
	int fd;
	char *path;

	if (argc != 2) {
		printf("invalid argment. %s ostype-path\n", argv[0]);
		return -4;
	}

	path = argv[1];

	if ((fd=open(path, O_RDONLY)) == -1 /* error */) {
		perror("lv15_map_file[open]:");
		return -3;
	}

	addr = mmap(NULL, len, prot, flag, fd, 0);
	if (addr == (void*)-1) {
		perror("lv15_map_file[mmap]:");
		return -2;
	}

	print = addr + len - 8;
	printf("mmap[%p-%p] %p=0x%lx\n",
		   addr, addr+len, print, *print);
	
	if (munmap(addr, len) == -1) {
		perror("lv15_map_file[munmap]:");
		return -1;
	}
	return 0;
}
