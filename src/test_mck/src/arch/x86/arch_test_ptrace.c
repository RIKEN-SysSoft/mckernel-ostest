/* arch_test_ptrace.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "arch_test_ptrace.h"

unsigned long get_or_do_inst(int mode)
{
	unsigned long addr = -1;

	switch (mode) {
	case MODE_GET_INST_ADDR:
		/* get label "1f" addr */
		break;
	case MODE_DO_INST:
		asm volatile(
			"nop\n"
			"nop\n"
			"1:\n"
			"nop\n"
			"nop\n"
			: /* nothing */
			: /* nothing */
		);
		addr = 0;
		break;

	default:
		break;
	}
	return addr;
}

unsigned long get_or_do_store_inst(int mode, unsigned long *buf, unsigned long change)
{
	return -1;
}

void pre_getregset(void)
{
	return;
}

int getregset_check(pid_t cpid)
{
	return -1;
}

int setregset_check(pid_t cpid, unsigned long *gregno, unsigned long *fregno)
{
	return -1;
}

int setregset_child_check(unsigned long gregno, unsigned long fregno)
{
	return -1;
}

int set_hw_watchpt(pid_t cpid, unsigned long addr)
{
	return -1;
}

int clear_hw_watchpt(pid_t cpid)
{
	return -1;
}

int check_trap_addr(pid_t cpid, unsigned long addr)
{
	return -1;
}

int set_hw_breakpt(pid_t cpid, unsigned long addr)
{
	return -1;
}

int clear_hw_breakpt(pid_t cpid)
{
	return -1;
}

int get_check_syscallno(pid_t cpid, long expno)
{
	return -1;
}
