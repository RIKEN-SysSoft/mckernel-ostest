/* 007.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "testsuite.h"

static int shmid = -1;
static char *shmaddr = NULL;


SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
#if ENABLE_LARGEST_PAGE
	unsigned long length = LARGEST_PAGE_SIZE * 2;
	unsigned long pgsize_log = LARGEST_PAGE_SHIFT << MAP_HUGE_SHIFT;
	int large_page_flag;
	int i;

	shmid = shmget(IPC_PRIVATE, length, pgsize_log | SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
	tp_assert(0 <= shmid, "shmget error.");
	printf("shmid: 0x%x\n", shmid);

	shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
	tp_assert(shmaddr != (char*)-1, "shmgat error.");
	printf("shmaddr: %p\n", shmaddr);

	for (i = 0; i < length; i += LARGE_PAGE_SIZE) {
		shmaddr[i] = (char)(i);
	}

        large_page_flag = check_page_size((unsigned long)shmaddr, LARGEST_PAGE_SIZE);
	// 注意：物理メモリのフラグメントが酷い場合にはノーマルページになるが、その場合は正常。(ページサイズは自動的に決まる)
	tp_assert(large_page_flag != 0, "invalid page size?");
#endif
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (shmaddr != NULL) {
		shmdt(shmaddr);
	}
	if (0 <= shmid) {
		shmctl(shmid, IPC_RMID, NULL);
	}
}
