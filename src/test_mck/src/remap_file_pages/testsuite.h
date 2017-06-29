/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

struct remap_file_pages_args {
	const char* dir;
	size_t file_size;
};

const char* remap_file_pages_create_file(char *path, size_t size);
char* remap_file_pages_pgoff_to_addr(char *start, int page);
const char* remap_file_pages_compare(char *remap_data, size_t size);

#endif /*__TEST_SUITE_H__*/
