/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "test_mck.h"
#include "testsuite.h"

static struct filemap_params {
	int tc_num;
	int open_flags;
	int mmap_prot;
	int mmap_flags;
	void(*cb)(void*,char*,size_t);
} filemap_params [] = {
	{ 0, O_RDONLY,	PROT_EXEC,	MAP_SHARED,  filemap_cb_read},
	{ 1, O_RDONLY,	PROT_EXEC,	MAP_SHARED,  filemap_cb_write},
	{ 2, O_RDONLY,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_read},
	{ 3, O_RDONLY,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_write},
	{ 4, O_RDONLY,	PROT_READ,	MAP_SHARED,  filemap_cb_read},
	{ 5, O_RDONLY,	PROT_READ,	MAP_SHARED,  filemap_cb_write},
	{ 6, O_RDONLY,	PROT_READ,	MAP_PRIVATE, filemap_cb_read},
	{ 7, O_RDONLY,	PROT_READ,	MAP_PRIVATE, filemap_cb_write},
	{ 8, O_RDONLY,	PROT_WRITE,	MAP_SHARED,  filemap_cb_read},
	{ 9, O_RDONLY,	PROT_WRITE,	MAP_SHARED,  filemap_cb_write},
	{10, O_RDONLY,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_read},
	{11, O_RDONLY,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_write},
	{12, O_RDONLY,	PROT_NONE,	MAP_SHARED,  filemap_cb_read},
	{13, O_RDONLY,	PROT_NONE,	MAP_SHARED,  filemap_cb_write},
	{14, O_RDONLY,	PROT_NONE,	MAP_PRIVATE, filemap_cb_read},
	{15, O_RDONLY,	PROT_NONE,	MAP_PRIVATE, filemap_cb_write},
	{16, O_WRONLY,	PROT_EXEC,	MAP_SHARED,  filemap_cb_read},
	{17, O_WRONLY,	PROT_EXEC,	MAP_SHARED,  filemap_cb_write},
	{18, O_WRONLY,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_read},
	{19, O_WRONLY,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_write},
	{20, O_WRONLY,	PROT_READ,	MAP_SHARED,  filemap_cb_read},
	{21, O_WRONLY,	PROT_READ,	MAP_SHARED,  filemap_cb_write},
	{22, O_WRONLY,	PROT_READ,	MAP_PRIVATE, filemap_cb_read},
	{23, O_WRONLY,	PROT_READ,	MAP_PRIVATE, filemap_cb_write},
	{24, O_WRONLY,	PROT_WRITE,	MAP_SHARED,  filemap_cb_read},
	{25, O_WRONLY,	PROT_WRITE,	MAP_SHARED,  filemap_cb_write},
	{26, O_WRONLY,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_read},
	{27, O_WRONLY,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_write},
	{28, O_WRONLY,	PROT_NONE,	MAP_SHARED,  filemap_cb_read},
	{29, O_WRONLY,	PROT_NONE,	MAP_SHARED,  filemap_cb_write},
	{30, O_WRONLY,	PROT_NONE,	MAP_PRIVATE, filemap_cb_read},
	{31, O_WRONLY,	PROT_NONE,	MAP_PRIVATE, filemap_cb_write},
	{32, O_RDWR,	PROT_EXEC,	MAP_SHARED,  filemap_cb_read},
	{33, O_RDWR,	PROT_EXEC,	MAP_SHARED,  filemap_cb_write},
	{34, O_RDWR,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_read},
	{35, O_RDWR,	PROT_EXEC,	MAP_PRIVATE, filemap_cb_write},
	{36, O_RDWR,	PROT_READ,	MAP_SHARED,  filemap_cb_read},
	{37, O_RDWR,	PROT_READ,	MAP_SHARED,  filemap_cb_write},
	{38, O_RDWR,	PROT_READ,	MAP_PRIVATE, filemap_cb_read},
	{39, O_RDWR,	PROT_READ,	MAP_PRIVATE, filemap_cb_write},
	{40, O_RDWR,	PROT_WRITE,	MAP_SHARED,  filemap_cb_read},
	{41, O_RDWR,	PROT_WRITE,	MAP_SHARED,  filemap_cb_write},
	{42, O_RDWR,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_read},
	{43, O_RDWR,	PROT_WRITE,	MAP_PRIVATE, filemap_cb_write},
	{44, O_RDWR,	PROT_NONE,	MAP_SHARED,  filemap_cb_read},
	{45, O_RDWR,	PROT_NONE,	MAP_SHARED,  filemap_cb_write},
	{46, O_RDWR,	PROT_NONE,	MAP_PRIVATE, filemap_cb_read},
	{47, O_RDWR,	PROT_NONE,	MAP_PRIVATE, filemap_cb_write},
	{48, O_RDWR,	PROT_WRITE,	MAP_SHARED,  filemap_cb_read_write},
};

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct filemap_args* args;
	args = filemap_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct filemap_args* args;
	struct filemap_params* param;
	int i;

	args = tc_arg;
	tp_assert(args->file != NULL, "invalid file path.");

	for (i = 0; i < sizeof(filemap_params)/sizeof(filemap_params[0]); i++) {
		if (filemap_params[i].tc_num == tc_num) {
			param = &filemap_params[i];
			break;
		}
	}
	tp_assert(tc_num != sizeof(filemap_params)/sizeof(filemap_params[0]), "invalid tc#.");

	return filemap_do_map(args->file, param->open_flags,
				   param->mmap_prot, param->mmap_flags,
				   param->cb, args);
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
