/* 003.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <unistd.h>
#include "test_mck.h"
#include "testsuite.h"

static char* before = (void*)-1;
#if ENABLE_LARGEST_PAGE
static char* addr = (void*)-1;
static const size_t increment = LARGEST_PAGE_SIZE;
#endif /*ENABLE_LARGEST_PAGE*/

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
#if ENABLE_LARGEST_PAGE
    char* start = NULL;
    char* end = NULL;
    char* aligned_addr = NULL;
    int large_page_flag = -1;

    /* sbrk */
    before = sbrk(0);
    printf("sbrk(0)=%p\n", before);
    tp_assert(before != (void*)-1, "sbrk error.");

    aligned_addr = (void*)align_up((unsigned long)before, LARGE_PAGE_SIZE);
    sbrk(aligned_addr - before);

    addr = sbrk(increment);
    printf("sbrk=%p\n", addr);
    tp_assert(addr == aligned_addr, "sbrk error.");
		
    /* access */
    start = addr;
    end = start + increment;
    tp_assert(start < end, "address overflow.");

    while (start < end) {
        unsigned long tte;
	unsigned long phys;
	/* store */
        *start = 'Z';
#if 0 /* for fx10 */
	/* check tte */
	tte = mck_va2pte((unsigned long)start);
	tp_assert(mck_pte_is_valid(tte) != 0, "tte valid.");
	/* check page size*/
	phys = tte & MCK_PAGE_PADDR;
	if (is_largest_page_alien(start) && is_largest_page_alien(phys)) {
		if (large_page_flag == -1) {
			unsigned long pgsz = mck_pte_get_page_size(tte);
			if (pgsz == LARGEST_PAGE_SIZE) {
				large_page_flag = 1;
			}
			else {
				large_page_flag = 0;
			}
		}
	}
#else /* for fx100 */
	UNUSED_VARIABLE(tte);
	UNUSED_VARIABLE(phys);
        large_page_flag = check_page_size((unsigned long)start, LARGEST_PAGE_SIZE);
        if (!large_page_flag) {
            break;
        }
#endif
	/* next */
        start += PAGE_SIZE;
    }
    // 注意：物理メモリのフラグメントが酷い場合にはノーマルページになるが、その場合は正常。(ページサイズは自動的に決まる)
    tp_assert(large_page_flag != 0, "invalid page size?.");
#endif /*ENABLE_LARGEST_PAGE*/
    return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
    /* free */
    if (before != (void*)-1) {
        brk(before);
    }
}

