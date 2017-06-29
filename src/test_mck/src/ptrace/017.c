/* 017.c COPYRIGHT FUJITSU LIMITED 2016 */
/* ptrace(PTRACE_SINGLESTEP) testcase */
#include "test_mck.h"
#include "arch_test_ptrace.h"
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
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
	if (addr == -1) {
		printf("get_or_do_rewrite_inst(MODE_GET_INST_ADDR) failed.\n");
		sem_post(swake);
		goto out;
	}

	/* semaphore wakeup */
	sem_post(swake);

	/* semaphore wait */
	sem_wait(swait);

	/* do instruction */
	if (get_or_do_inst(MODE_DO_INST)) {
		printf("get_or_do_rewrite_inst(MODE_DO_INST) failed.\n");
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
	int i = 0;
	const int loop_count = 5;
	long read_val = 0;
	long write_val = 0;
	long inst_val = 0;

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
	inst_val = ptrace(PTRACE_PEEKTEXT, cpid, read_val, NULL);
	if ((read_val == -1) && errno) {
		perror("ptrace(PTRACE_PEEKTEXT)");
		goto cont;
	}

	/* generate write value */
	write_val = ((inst_val & 0xffffffff00000000UL) | BRK_INST);

	/* write child brk instruction */
	if (ptrace(PTRACE_POKETEXT, cpid, read_val, write_val)) {
		perror("ptrace(PTRACE_POKETEXT)");
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
			if (WSTOPSIG(status) != SIGTRAP) {
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

	/* rewrite child instruction */
	if (ptrace(PTRACE_POKETEXT, cpid, read_val, inst_val)) {
		perror("ptrace(PTRACE_POKETEXT)");
		goto cont;
	}

	/* single step loop */
	for (i = 0, read_val += 4; i < loop_count; i++, read_val += 4) {
		printf("[count%02d]\n", i);

		/* child continue (until next instruction) */
		if (ptrace(PTRACE_SINGLESTEP, cpid, NULL, NULL)) {
			perror("  ptrace(PTRACE_SINGLESTEP)");
			goto cont;
		}

		/* wait child stop */
		pid = wait(&status);
		if (pid == cpid) {
			if (WIFSTOPPED(status)) {
				if (WSTOPSIG(status) == SIGTRAP) {
					siginfo_t info;

					printf("  child is stopped by SIGTRAP.\n");

					/* read child siginfo */
					if (ptrace(PTRACE_GETSIGINFO, cpid, 0, &info)) {
						perror("  ptrace(PTRACE_GETSIGINFO)");
						goto cont;
					}

					/* check si_signo */
					if (info.si_signo == SIGTRAP) {
						printf("  child si_signo == SIGTRAP\n");
					} else {
						printf("  child si_signo(%d) != SIGTRAP(%d)\n", info.si_signo, SIGTRAP);
						goto cont;
					}

					/* check si_errno */
					if (info.si_errno == 0) {
						printf("  child si_errno == 0\n");
					} else {
						printf("  child si_errno(%d) != 0\n", info.si_errno);
						goto cont;
					}

#ifndef TRAP_HWBKPT
#define TRAP_HWBKPT 4
#endif /* !TRAP_HWBKPT */
					/* check si_code */
					if (info.si_code == TRAP_HWBKPT) {
						printf("  child si_code == TRAP_HWBKPT\n");
					} else {
						printf("  child si_code(%d) != TRAP_HWBKPT(%d)\n", info.si_code, TRAP_HWBKPT);
						goto cont;
					}

					/* check si_addr */
					if ((unsigned long)info.si_addr == read_val) {
						printf("  child is stopped singlestep addr.\n");
					} else {
						printf("  child is stopped not singlestep addr.\n");
						printf("  expection:0x%lx, info.si_addr:0x%lx\n", read_val, (unsigned long)info.si_addr);
						goto cont;
					}
				} else {
					printf("  child is stopped by not SIGTRAP (%d).\n", WSTOPSIG(status));
					goto cont;
				}
			} else {
				printf("  child is not stopped.\n");
				goto out;
			}
		} else {
			perror("  wait()");
			goto out;
		}
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
