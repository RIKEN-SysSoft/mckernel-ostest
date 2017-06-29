/* test_mck.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_MCK_H__
#define __TEST_MCK_H__

#include <stdio.h>
#include <sys/types.h>
#include "arch_test_mck.h"
#include "test_mck_drv.h"

extern char* the_app;

/* test case interface */
#define SETUP_NAME(ts, num)    ts ## num ## _setup
#define RUN_NAME(ts, num)      ts ## num
#define TEARDOWN_NAME(ts, num) ts ## num ## _teardown

#define SETUP_FUNC(ts, num)    void* SETUP_NAME(ts, num)(int tc_num, int tc_argc, char** tc_argv)
#define RUN_FUNC(ts, num)      const char* RUN_NAME(ts, num)(int tc_num, void* tc_arg)
#define TEARDOWN_FUNC(ts, num) void TEARDOWN_NAME(ts, num)(int tc_num, void* tc_arg)

#define SETUP_EMPTY(ts, num) SETUP_FUNC(ts, num){return NULL;}
#define RUN_EMPTY(ts,num) RUN_FUNC(ts, num){return NULL;}
#define TEARDOWN_EMPTY(ts, num) TEARDOWN_FUNC(ts, num){}

#define SETUP_DELEGATION(ts, num, _num, _argc, _argv)    SETUP_NAME(ts, num)(_num, _argc, _argv)
#define RUN_DELEGATION(ts, num, _num, _arg)      RUN_NAME(ts, num)(_num, _arg)
#define TEARDOWN_DELEGATION(ts, num, _num, _arg) TEARDOWN_NAME(ts, num)(_num, _arg)

#define SETUP_ALIAS(ts, alias_num, num)		\
	SETUP_FUNC(ts, alias_num) {				\
		extern SETUP_FUNC(ts, num);			\
		return SETUP_DELEGATION(ts, num, alias_num, tc_argc, tc_argv);	\
	}

#define RUN_ALIAS(ts, alias_num, num)		\
	RUN_FUNC(ts, alias_num) {				\
		extern RUN_FUNC(ts, num);			\
		return RUN_DELEGATION(ts, num, alias_num, tc_arg);    \
	}

#define TEARDOWN_ALIAS(ts, alias_num, num)		\
	TEARDOWN_FUNC(ts, alias_num) {				\
		extern TEARDOWN_FUNC(ts, num);			\
		TEARDOWN_DELEGATION(ts, num, alias_num, tc_arg);	\
	}

/* util */
#define tp_assert(test, msg) _tp_assert(TEST_SUITE, TEST_NUMBER, __LINE__, test, msg)
#define _tp_assert(ts, num, line, test, msg) __tp_assert(ts, num, line, test, msg)
//#define __tp_assert(ts, num, line, test, msg) do{ if(!(test)) return #ts "#" #num "(" #line ") - " msg;}while(0)
#define __tp_assert(ts, num, line, test, msg) do{ if(!(test)) return msg;}while(0)

#define align_down(addr, size)  ((addr)&(~((size)-1)))
#define align_up(addr, size)    align_down((addr) + (size) - 1, size)

#define TEMP_FILE_PREFIX _TEMP_FILE_PREFIX(TEST_SUITE, TEST_NUMBER)
#define _TEMP_FILE_PREFIX(suite, num) __TEMP_FILE_PREFIX(suite, num)
#define __TEMP_FILE_PREFIX(suite, num) #suite #num
char* test_mck_create_temp_file(const char* prefix, size_t size);
void test_mck_delete_temp_file(char* path);

unsigned long test_mck_strtoul(const char *nptr, unsigned long default_value);

#ifndef get_stack_addr
#define get_stack_addr(sp) -1
#endif /* !get_stack_addr */

#define UNUSED_VARIABLE(v)  (void)(v)

/* memory */
#define PAGE_SIZE    (1UL << PAGE_SHIFT)
#define PAGE_OFFSET  (PAGE_SIZE - 1UL)
#define PAGE_MASK    (~PAGE_OFFSET)

#define LARGE_PAGE_SIZE    (1UL << LARGE_PAGE_SHIFT)
#define LARGE_PAGE_OFFSET  (LARGE_PAGE_SIZE - 1UL)
#define LARGE_PAGE_MASK    (~LARGE_PAGE_OFFSET)

#define LARGEST_PAGE_SIZE    (1UL << LARGEST_PAGE_SHIFT)
#define LARGEST_PAGE_OFFSET  (LARGEST_PAGE_SIZE - 1UL)
#define LARGEST_PAGE_MASK    (~LARGEST_PAGE_OFFSET)

#define PAGE_ALIGN(addr) align_up(addr, PAGE_SIZE)
#define LARGE_PAGE_ALIGN(addr) align_up(addr, LARGE_PAGE_SIZE)
#define LARGEST_PAGE_ALIGN(addr) align_up(addr, LARGEST_PAGE_SIZE)

#define TOFU_MAPPING_START 			0xFFFFF80100020000UL
#define TOFU_MAPPING_END 			0xFFFFF80100040000UL
#define TOFU_MAPPING_ESERVED_START 	0xFFFFF80100040000UL
#define TOFU_MAPPING_RESERVED_END 	0xFFFFF80100050000UL
#define VMEM_SPACE_HOLE_START 		0x000007EF00000000UL
#define VMEM_SPACE_HOLE_END 		0xFFFFF80100000000UL
#define TASK_UNMAPPED_BASE 			0xFFFFF80100050000UL
#define USER_END 				0xFFFFFFFFFFFFC000UL

/* Special system call for mckernel */
#define __NR_get_cpu_id		700
extern int get_cpu_id(void);

/* string */
#define STRING(str)  _STRING(str)
#define _STRING(str) #str

/* wrapper */
pid_t gettid(void);

#endif /*__TEST_MCK_H__*/
