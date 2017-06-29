/* lv15_map_anon.c COPYRIGHT FUJITSU LIMITED 2015-2016 */

#include <stdio.h>
#include <sys/mman.h>

#define TASK_UNMAPPED_BASE 0x0000007FE0000000UL

#define _4MB  (1024UL * 1024UL * 4UL)
#define _256MB  (1024UL * 1024UL * 256UL)

#define ALIGN_UP(a, b)  ( ((a)+(b)-1) & ~((b)-1) )

int main(int argc, char** argv)
{
	/*
	 * - anonymous mapping
	 * - 4MB
	 * - ld
	 */
	size_t len = _4MB;
	char *addr;
	char *print;
	int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	int flag = MAP_PRIVATE | MAP_ANONYMOUS;

	addr = mmap(NULL, len, prot, flag, -1, 0);
	if (addr == (void*)-1) {
		perror("lv15_map_anon[mmap]:");
		return -1;
	}

	print = addr + len - 8;
	printf("mmap[%p-%p] %p=0x%lx\n",
		   addr, addr+len, print, *print);
	
	if (munmap(addr, len) == -1) {
		perror("lv15_map_anon[munmap]:");
		return -1;
	}
	return 0;
}
