/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#if defined(__sparc)
#  include "mck.h"
#else
#  include <errno.h>
#  define mck_pte_is_valid(pte) (0)
#  define MCK_PAGE_PADDR (0)

#ifndef MAP_HUGE_SHIFT
# define MAP_HUGE_SHIFT 26
#endif

#ifndef SHM_HUGETLB
# define SHM_HUGETLB 04000
#endif

/* Only ia64 requires this */
#ifdef __ia64__
# define ADDR (void *)(0x8000000000000000UL)
# define SHMAT_FLAGS (SHM_RND)
#else
# define ADDR (void *)(0x0UL)
# define SHMAT_FLAGS (0)
#endif

static inline unsigned long mck_va2pte(unsigned long va)
{
	return 0;
}


static inline int mck_pte_get_phys(unsigned long pte, unsigned long *phys)
{
	return -EFAULT;
}

static inline long mck_pte_get_page_size(unsigned long pte)
{
	return -EFAULT;
}

#endif

extern int check_page_size(unsigned long va, unsigned long pagesize);

#define is_large_page_alien(addr)   (((unsigned long)(addr) & LARGE_PAGE_OFFSET) == 0)
#define is_largest_page_alien(addr) (((unsigned long)(addr) & LARGEST_PAGE_OFFSET) == 0)

#endif /*__TEST_SUITE_H__*/
