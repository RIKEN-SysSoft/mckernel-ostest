/* 003.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_PEEKTEXT) testcase */
#include "test_mck.h"
#include "arch_test_ptrace.h"
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static unsigned long addr = 0;

static int child_func(sem_t *swait, sem_t *swake)
{
	int ret = -1;

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* read nop instruction addr */
	addr = get_or_do_inst(MODE_GET_INST_ADDR);

	/* semaphore wakeup */
	sem_post(swake);

	/* semaphore wait */
	sem_wait(swait);

	/* success */
	ret = 0;
out:
	return ret;
}

static int parent_func(sem_t *swait, sem_t *swake, pid_t cpid)
{
	pid_t pid = 0;
	int status = 0;
	int ret = -1;
	long read_val = 0;
	unsigned int inst_code = 0;

	/* semaphore wait */
	sem_wait(swait);

	/* send SIGSTOP signal */
	if (kill(cpid, SIGSTOP)) {
		perror("kill()");
		goto out;
	}

	/* semaphore wakeup */
	sem_post(swake);

	/* wait child stop */
	pid = wait(&status);
	if (pid == cpid) {
		if (!WIFSTOPPED(status)) {
			printf("child is not stopped.\n");
			goto out;
		}
	} else {
		perror("wait()");
		goto out;
	}

	/* read child nop address */
	read_val = ptrace(PTRACE_PEEKDATA, cpid, &addr, NULL);
	if ((read_val == -1) && errno) {
		perror("ptrace(PTRACE_PEEKDATA)");
		goto cont;
	}

	/* read child nop instruction */
	read_val = ptrace(PTRACE_PEEKTEXT, cpid, read_val, NULL);
	if ((read_val == -1) && errno) {
		perror("ptrace(PTRACE_PEEKTEXT)");
		goto cont;
	}

	/* check read nop instructoon code */
	inst_code = (unsigned int)(read_val >> 32UL);
	if (inst_code == NOP_INST) {
		printf("PTRACE_PEEKTEXT inst_code(0x%x) == 0x%lx\n", inst_code, NOP_INST);
	} else {
		printf("PTRACE_PEEKTEXT inst_code(0x%x) != 0x%lx\n", inst_code, NOP_INST);
		goto cont;
	}

	/* success */
	ret = 0;
cont:
	/* child continue */
	if (ptrace(PTRACE_CONT, cpid, NULL, NULL)) {
		perror("ptrace(PTRACE_CONT)");
		ret = -1;
	}
out:
	return ret;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	pid_t pid = 0;
	sem_t *pwait = NULL;
	sem_t *cwait = NULL;
	int ret = 0;
	int func_ret = 0;
	int status = 0;

	/* get sync semaphore mapping */
	pwait = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	cwait = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	/* mmap error check */
	if (!pwait || !cwait) {
		perror("mmap()");
		return "mmap() failed.";
	}

	/* sync semaphore initialize */
	ret |= sem_init(pwait, 1, 0);
	ret |= sem_init(cwait, 1, 0);

	/* sem_init error check */
	if (ret) {
		perror("sem_init()");
		return "sem_init() failed.";
	}

	/* create child process */
	pid = fork();

	switch (pid) {
	case -1:
		/* fork() error. */
		perror("fork()");
		return "fork() failed.";
		break;
	case 0:
		/* child process */
		func_ret = child_func(cwait, pwait);

		/* sync semaphore unmap */
		ret |= munmap(cwait, sizeof(sem_t));
		ret |= munmap(pwait, sizeof(sem_t));

		/* munmap error check */
		if (ret) {
			perror("[child] munmap()");
			func_ret = -1;
		}

		/* child exit */
		exit(func_ret);
		break;
	default:
		/* parent process */
		func_ret = parent_func(pwait, cwait, pid);

		/* sync semaphore unmap */
		ret |= munmap(cwait, sizeof(sem_t));
		ret |= munmap(pwait, sizeof(sem_t));

		/* munmap error check */
		if (ret) {
			perror("[parent] munmap()");
			return "munmap() failed.";
		}

		/* wait child */
		pid = wait(&status);
		if (pid != -1) {
			if (WEXITSTATUS(status)) {
				return "TP failed.";
			}
		} else {
			perror("wait()");
			return "wait() failed.";
		}

		/* parent_func check */
		if (func_ret) {
			return "TP failed.";
		}
		break;
	}

	/* sccess. */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
