/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#define TEST_SHM_FLAG (int)(IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	char *curdir = NULL;
	key_t shm_key;
	int shm_id = 0;
	void *shm_addr = NULL;
	int result;

	/* generate System V IPC key */
	curdir = getcwd(curdir, (size_t)0 /* auto allocate curdir buffer in getcwd */);
	printf("curdir = %s\n", curdir);
	shm_key = ftok(curdir, tc_num);
	tp_assert(shm_key != (key_t)-1, "ftok FAILED");
	printf("ftok => shm_key = %d\n", shm_key);

	// int shmget(key_t key, size_t size, int shmflg);
	result = shmget(shm_key, SHMLBA, TEST_SHM_FLAG);
	tp_assert(result != -1, "shmget FAILED");
	shm_id = result;
	printf("shmget => shm_id = %d\n", shm_id);

	// void *shmat(int shmid, const void *shmaddr, int shmflg);
	shm_addr = shmat(shm_id, NULL, SHM_RND);
	tp_assert (shm_addr != (void *)-1, "shmat FAILED");
	printf("shmat => shm_addr = %p\n", shm_addr);

	// int shmdt(const void *shmaddr);
	result = shmdt(shm_addr);
	tp_assert (result != -1, "shmdt FAILED");
	printf("shmdt => result = %d\n", result);

	// int shmctl(int shmid, int cmd, struct shmid_ds *buf);
	result = shmctl(shm_id, IPC_RMID, NULL);
	tp_assert (result != -1, "shmctl FAILED");
	printf("shmctl => result = %d\n", result);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
