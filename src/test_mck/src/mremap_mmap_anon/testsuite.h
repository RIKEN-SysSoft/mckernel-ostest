/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#define MREMAP_RANGE_SIZE (PAGE_SIZE * 4)
#define MREMAP_RANGE_LARGE_SIZE align_up(LARGE_PAGE_SIZE, MREMAP_RANGE_SIZE)

struct mremap_args {
	unsigned long text;
	unsigned long text_len;
	unsigned long data;
	unsigned long data_len;
	unsigned long bss;
	unsigned long bss_len;
};

struct mremap_args* mremap_mmap_anon_parse_argument(char** argv, int argc);
const char* mremap_mmap_anon_data_check(const void* addr, size_t len, char exp);
const char* mremap_mmap_anon_verify_section_argument(const struct mremap_args* args);

struct mremap_mmap_anon_param {
	char* addr;  //[OUT] mapping address.
	size_t size; //[IN]  mmap size.
	int nopage;  //[IN]  free space flag. [0: use space, 1:free space]
	// use space param
	int shared;  //[IN]  sharead mapping. [0:private, 1:shared]
	int prot;    //[IN]  mmap prot.
};
size_t mremap_mmap_anon(struct mremap_mmap_anon_param params[], size_t count);
const char* mremap_mmap_anon_access_check(void* addr, size_t len);

const char* mremap_mmap_anon_setup_signal_handler(int signum);

#endif /*__TEST_SUITE_H__*/
