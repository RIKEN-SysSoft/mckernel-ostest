/* 999.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

struct process_vm_writev_args
{
	unsigned long addr;
	size_t size;
	int expr;
};

static char* write_buf;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct process_vm_writev_args args;
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
	struct process_vm_writev_args* args;
	struct iovec local[1];
	struct iovec remote[1];
	ssize_t nwrite;
	pid_t pid;
	unsigned long flags = 0;

	/* init */
	args = (struct process_vm_writev_args*)tc_arg;
	write_buf = malloc(args->size);
	tp_assert(write_buf, "malloc error(read).");
	memset(write_buf, args->expr, args->size);

	/* write */
	pid = getppid();
	local[0].iov_base = write_buf;
	local[0].iov_len = args->size;
	remote[0].iov_base = (void*)args->addr;
	remote[0].iov_len = args->size;
	D("pid:%d, write_buf:%p, args->addr:%p, size:%#lx\n",
	      pid, write_buf, (void*)args->addr, args->size);

	nwrite = call_process_vm_writev(pid,
					local, sizeof(local)/sizeof(local[0]),
					remote, sizeof(remote)/sizeof(remote[0]), flags);
	tp_assert(nwrite != -1, "process_vm_readv error.");
	tp_assert(nwrite == args->size, "invalid read size.");
	D("success\n");
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (write_buf) {
		free(write_buf);
	}
}
