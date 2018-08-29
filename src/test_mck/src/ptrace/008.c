/* 008.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_GETREGSET) testcase */
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fenv.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/user.h> /* for GETFPREGS */
#include "test_mck.h"
#include "arch_test_ptrace.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static int child_func(sem_t *swait, sem_t *swake)
{
	int ret = -1;

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* pre write register */
	pre_getregset();

	/* semaphore wakeup */
	sem_post(swake);

	/* change current cpu's fpregs */
	feraiseexcept(FE_ALL_EXCEPT);

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
	struct user_fpregs_struct fpregs;

	/* semaphore wait */
	sem_wait(swait);

	/* wait for child's fpregs is changed */
	usleep(1000);

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

	/* get child's fpregs */
	if (ptrace(PTRACE_GETFPREGS, cpid, NULL, &fpregs)) {
		perror("ptrace(PTRACE_GETFPREGS)");
		goto cont;
	}

	/* check child's fpregs */
	printf("child's fpregs.swd is 0x%x (expected NOT ZERO)\n", fpregs.swd);
	if (fpregs.swd == 0) {
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

	/* change current cpu's fpregs */
	feclearexcept(FE_ALL_EXCEPT);

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
