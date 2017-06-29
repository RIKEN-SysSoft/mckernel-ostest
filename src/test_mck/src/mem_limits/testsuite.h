/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

enum mem_limits_function {
	MEM_LIMITS_FN_NULL,
	MEM_LIMITS_FN_MALLOC,
	MEM_LIMITS_FN_MMAP,
	MEM_LIMITS_FN_BRK,
};

extern long long MEM_LIMITS_COUNT_INFINITE;

struct mem_limits_args {
	enum mem_limits_function fn;
	size_t size;
	long long count;
};

#endif /*__TEST_SUITE_H__*/
