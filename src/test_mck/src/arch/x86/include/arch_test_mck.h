/* arch_test_mck.h COPYRIGHT FUJITSU LIMITED 2015-2016 */

#ifndef __X86_ARCH_TEST_MCK_H
#define __X86_ARCH_TEST_MCK_H

/* auxv */
extern char *auxv_name[];

/* memory */
#define PAGE_SHIFT		12
#define LARGE_PAGE_SHIFT	21
#define LARGEST_PAGE_SHIFT	30

#define MMAP_START_ADDR	0x00002AAAAAA00000UL	/* in McKernel ((USER_END / 3) & LARGE_PAGE_MASK) value */
#define MMAP_END_ADDR	0x0000800000000000UL    /* in McKernel USER_END value */
#define MMAP_AREA_SIZE	(MMAP_END_ADDR - MMAP_START_ADDR)

/* asm code */
#define get_stack_addr()						\
	({								\
		unsigned long sp;					\
		__asm__ __volatile__("movq %%rsp, %0" : "=r"(sp));	\
		sp;							\
	})

#define cpu_pause()						\
	({							\
		__asm__ __volatile__("pause" ::: "memory");	\
	})

#define register_write() while(0)

/* syscall number */
#ifndef __NR_getcpu
# define __NR_getcpu 318
#endif

#ifndef __NR_process_vm_readv
# ifdef __x86_64
#  define __NR_process_vm_readv 347
# else
#  define __NR_process_vm_readv 310
# endif
#endif

#ifndef __NR_process_vm_writev
# ifdef __x86_64
#  define __NR_process_vm_writev 348
# else
#  define __NR_process_vm_writev 311
# endif
#endif

#endif /* __X86_ARCH_TEST_MCK_H */
