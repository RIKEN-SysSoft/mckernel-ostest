/* 999.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

struct process_vm_readv_args
{
	unsigned long addr;
	size_t size;
	int expr;
};

static char* read_buf;
static char* expr_buf;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct process_vm_readv_args args;
	char* ch;
	int opt;
	
	/*
	  a: parent process memory address.
	  s: address space size.
	  e: expectation value.
	*/
	while ((opt = getopt(tc_argc, tc_argv, "a:s:e:")) != -1) {
		switch (opt) {
		case 'a':
			args.addr = strtoul(optarg, &ch, 0);
			break;
		case 's':
			args.size = strtoul(optarg, &ch, 0);
			break;
		case 'e':
			args.expr = optarg[0];
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct process_vm_readv_args* args;
	struct iovec local[1];
	struct iovec remote[1];
	ssize_t nread;
	pid_t pid;
	unsigned long flags = 0;
	int cmp;

	/* init */
	args = (struct process_vm_readv_args*)tc_arg;
	read_buf = malloc(args->size);
	tp_assert(read_buf, "malloc error(read).");

	/* read */
	pid = getppid();
	local[0].iov_base = read_buf;
	local[0].iov_len = args->size;
	remote[0].iov_base = (void*)args->addr;
	remote[0].iov_len = args->size;
	D("pid:%d, read_buf:%p, args->addr:%p, size:%#lx\n",
	      pid, read_buf, (void*)args->addr, args->size);

	nread = call_process_vm_readv(pid,
				 local, sizeof(local)/sizeof(local[0]),
				 remote, sizeof(remote)/sizeof(remote[0]), flags);
	tp_assert(nread != -1, "process_vm_readv error.");
	tp_assert(nread == args->size, "invalid read size.");

	/* check */
	expr_buf = malloc(args->size);
	tp_assert(expr_buf, "malloc error(expr).");
	memset(expr_buf, args->expr, args->size);

	cmp = memcmp(read_buf, expr_buf, args->size);
	tp_assert(cmp == 0, "memcmp error.");
	D("success\n");
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (read_buf) {
		free(read_buf);
	}

	if (expr_buf) {
		free(expr_buf);
	}
}
