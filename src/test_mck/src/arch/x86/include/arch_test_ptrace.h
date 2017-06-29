/* arch_test_ptrace.h COPYRIGHT FUJITSU LIMITED 2016 */
#ifndef __X86_ARCH_TEST_PTRACE_H
#define __X86_ARCH_TEST_PTRACE_H

#define MODE_GET_INST_ADDR	1
#define MODE_DO_INST		2
#define MODE_DO_STORE_INST	3

#include <sys/types.h>

extern unsigned long get_or_do_inst(int mode);
extern unsigned long get_or_do_store_inst(int mode, unsigned long *buf, unsigned long change);
extern void pre_getregset(void);
extern int getregset_check(pid_t cpid);
extern int setregset_check(pid_t cpid, unsigned long *gregno, unsigned long *fregno);
extern int setregset_child_check(unsigned long gregno, unsigned long fregno);
extern int set_hw_watchpt(pid_t cpid, unsigned long addr);
extern int clear_hw_watchpt(pid_t cpid);
extern int check_trap_addr(pid_t cpid, unsigned long addr);
extern int set_hw_breakpt(pid_t cpid, unsigned long addr);
extern int clear_hw_breakpt(pid_t cpid);
extern int get_check_syscallno(pid_t cpid, long expno);

/* instruction code */
#define NOP_INST	0x00000000UL
#define BRK_INST	0x00000000UL

#endif /* !__X86_ARCH_TEST_PTRACE_H */
