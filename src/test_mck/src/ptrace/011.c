/* 011.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_SETREGSET) testcase */
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "test_mck.h"
#include "arch_test_ptrace.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

static unsigned long buf = 0;
static unsigned long addr = 0;

#define CHILD_MAGIC 0xcafecafecafecafeUL

static int child_func(sem_t *swait, sem_t *swake)
{
	int ret = -1;

	/* send PTRACE_TRACEME */
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
		perror("ptrace(PTRACE_TRACEME)");
		goto out;
	}

	/* get store instruction address */
	addr = get_or_do_store_inst(MODE_GET_INST_ADDR, NULL, 0);
	if (addr == -1) {
		printf("get_or_do_store_inst(MODE_GET_INST_ADDR) failed.\n");
		sem_post(swake);
		goto out;
	}

	/* semaphore wakeup */
	sem_post(swake);

	/* semaphore wait */
	sem_wait(swait);

	/* do store instruction */
	if (get_or_do_store_inst(MODE_DO_STORE_INST, &buf, CHILD_MAGIC)) {
		printf("get_or_do_store_inst(MODE_DO_STORE_INST) failed.\n");
		goto out;
	}

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
	unsigned long inst_addr = 0;

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

	/* read child store instruction address */
	inst_addr = ptrace(PTRACE_PEEKDATA, cpid, &addr, NULL);
	if ((inst_addr == -1) && errno) {
		perror("ptrace(PTRACE_PEEKDATA)");
		goto cont;
	}

	/* set hardware watchpoint */
	if (set_hw_watchpt(cpid, (unsigned long)&buf)) {
		printf("set_hw_watchpt failed.\n");
		goto cont;
	}

	/* child continue */
	if (ptrace(PTRACE_CONT, cpid, NULL, NULL)) {
		perror("ptrace(PTRACE_CONT)");
		goto out;
	}

	/* wait child stop */
	pid = wait(&status);
	if (pid == cpid) {
		if (WIFSTOPPED(status)) {
			if (WSTOPSIG(status) == SIGTRAP) {
				siginfo_t info;

				printf("child is stopped by SIGTRAP.\n");

				/* read child siginfo */
				if (ptrace(PTRACE_GETSIGINFO, cpid, 0, &info)) {
					perror("ptrace(PTRACE_GETSIGINFO)");
					goto cont;
				}

				/* check si_signo */
				if (info.si_signo == SIGTRAP) {
					printf("child si_signo == SIGTRAP\n");
				} else {
					printf("child si_signo(%d) != SIGTRAP(%d)\n", info.si_signo, SIGTRAP);
					goto cont;
				}

				/* check si_errno */
				if (info.si_errno == 0) {
					printf("child si_errno == 0\n");
				} else {
					printf("child si_errno(%d) != 0\n", info.si_errno);
					goto cont;
				}

				/* check si_code */
#ifndef TRAP_HWBKPT
#define TRAP_HWBKPT 4
#endif /* !TRAP_HWBKPT */
				if (info.si_code == TRAP_HWBKPT) {
					printf("child si_code == TRAP_HWBKPT\n");
				} else {
					printf("child si_code(%d) != TRAP_HWBKPT(%d)\n", info.si_code, TRAP_HWBKPT);
					goto cont;
				}

				/* check si_addr */
				if ((unsigned long)info.si_addr == (unsigned long)&buf) {
					printf("child si_addr == change value addr\n");
				} else {
					printf("child si_addr != change value addr\n");
					printf("expection:0x%lx, info.si_addr:0x%lx\n",
						(unsigned long)&buf, (unsigned long)info.si_addr);
					goto cont;
				}

				/* check trap addr */
				if (check_trap_addr(cpid, inst_addr)) {
					printf("check_trap_addr failed.\n");
					goto cont;
				}
			} else {
				printf("child is stopped by not SIGTRAP (%d).\n", WSTOPSIG(status));
				goto cont;
			}
		} else {
			printf("child is not stopped.\n");
			goto out;
		}
	} else {
		perror("wait()");
		goto out;
	}

	/* clear hardware watchpoint */
	if (clear_hw_watchpt(cpid)) {
		printf("clear_hw_watchpt failed.\n");
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
