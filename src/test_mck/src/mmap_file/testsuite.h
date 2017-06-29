/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

/*filemap.c*/
struct filemap_args {
	const char* file;
};

const char* filemap_do_map(
	const char* path, int open_flags,
	int mmap_prot, int mmap_flags,
	void(*cb)(void*,char*,size_t), void* cb_arg);
void filemap_cb_write(void* arg, char* buf, size_t sizs);
void filemap_cb_read(void* arg, char* buf, size_t sizs);
void filemap_cb_read_write(void* arg, char* buf, size_t size);
void* filemap_parse_args(int argc, char** argv);

#endif /*__TEST_SUITE_H__*/
