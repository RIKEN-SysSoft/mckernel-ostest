/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#if defined(__sparc)
#  include "mck.h"
#else
#  include <errno.h>
#  define mck_pte_is_valid(pte) (0)
#  define MCK_PAGE_PADDR (0)

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

#define is_large_page_alien(addr)   (((unsigned long)(addr) & LARGE_PAGE_OFFSET) == 0)
#define is_largest_page_alien(addr) (((unsigned long)(addr) & LARGEST_PAGE_OFFSET) == 0)

#endif /*__TEST_SUITE_H__*/
