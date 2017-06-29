/* mremap_mmap_anon.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <getopt.h>
#include <string.h>
#include <signal.h>

struct mremap_args* mremap_mmap_anon_parse_argument(char** argv, int argc)
{
	static struct mremap_args mremap_args = {0};

	int opt;
	int option_index;
	struct option long_options[] = {
		{"text", required_argument,  NULL, 't'},
		{"text-len", required_argument,  NULL, 'T'},
		{"data", required_argument,  NULL, 'd'},
		{"data-len", required_argument,  NULL, 'D'},
		{"bss", required_argument,  NULL, 'b'},
		{"bss-len", required_argument,  NULL, 'B'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "t:T:d:D:b:B:", long_options, &option_index)) != -1) {
		switch (opt) {
		case 't':
			mremap_args.text = test_mck_strtoul(optarg, 0);
			break;
		case 'T':
			mremap_args.text_len = test_mck_strtoul(optarg, 0);
			break;
		case 'd':
			mremap_args.data = test_mck_strtoul(optarg, 0);
			break;
		case 'D':
			mremap_args.data_len = test_mck_strtoul(optarg, 0);
			break;
		case 'b':
			mremap_args.bss = test_mck_strtoul(optarg, 0);
			break;
		case 'B':
			mremap_args.bss_len = test_mck_strtoul(optarg, 0);
			break;
		case '?':
			continue;
		}
	}
	return &mremap_args;
}

const char* mremap_mmap_anon_verify_section_argument(const struct mremap_args* args)
{
	tp_assert(args->text != 0,
		  "invalid argument(-t, --text).");
	tp_assert(args->text_len != 0,
		  "invalid argument(-T, --text-len).");

	tp_assert(args->data != 0,
		  "invalid argument(-d, --data).");
	tp_assert(args->data_len != 0,
		  "invalid argument(-D, --data-len).");

	tp_assert(args->bss != 0,
		  "invalid argument(-b, --bss).");
	tp_assert(args->bss_len != 0,
		  "invalid argument(-B, --bss-len).");
	return NULL;
}

static void mremap_mmap_anon_signal_handler(int sig, siginfo_t *sip, void *ucp)
{
	exit(0);
}

const char* mremap_mmap_anon_setup_signal_handler(int signum)
{
	struct sigaction sa;

	sa.sa_sigaction = mremap_mmap_anon_signal_handler;
	sa.sa_flags = SA_RESETHAND | SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	if (0 != sigaction(signum, &sa, NULL)) {
		return "sigaction(SIGSEGV) error.";
	}
	return NULL;
}

size_t mremap_mmap_anon(struct mremap_mmap_anon_param params[], size_t count)
{
	int i;
	size_t total_size;
	char* addr;

	// initialize
	total_size = 0;
	for (i = 0; i < count; i++) {
		size_t sum = total_size + params[i].size;
		if (sum < total_size) {
			return 0;
		}
		total_size = sum;
		params[i].addr = MAP_FAILED;
	}

	// reserve space
	addr = mmap(0, total_size, PROT_NONE,
		    MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED) {
		return 0;
	}

	// formatting space
	for (i = 0; i < count; i++) {
		params[i].addr = addr;
		if (params[i].nopage) {
			if (0 != munmap(addr, params[i].size)) {
				return i;
			}
		}
		else {
			int flags = MAP_ANONYMOUS|MAP_FIXED;
			if (params[i].shared) {
				flags |= MAP_SHARED;
			} else {
				flags |= MAP_PRIVATE;
			}

			addr = mmap(addr, params[i].size, params[i].prot,
				    flags, -1, 0);
			if (addr == MAP_FAILED) {
				return i;
			}
			if (params[i].prot & PROT_WRITE) {
				char ch = '0' + i;
				memset(addr, ch, params[i].size);
			}
		}
		addr += params[i].size;
	}
	return i;
}

const char* mremap_mmap_anon_data_check(const void* addr, size_t len, char exp)
{
	const char* pch;
	size_t idx;


	pch = (char*)addr;	
	for (idx = 0; idx < len; idx++) {
		if (pch[idx] != exp) {
			return "data check error.";
		}
	}
	return NULL;
}

const char* mremap_mmap_anon_access_check(void* addr, size_t len)
{
	char ch;
	char* pch;

	// arg check
	if (addr == NULL) {
		return "invalid address";
	}
	if (len <= 0) {
		return NULL;  // return sccess
	}

	pch = (char*)addr;

	// first page
	pch[0] = 'A';
	ch = pch[0];
	tp_assert(ch == 'A', "first page access error");

	// last page
	pch[len-1] = 'Z';
	ch = pch[len-1];
	tp_assert(ch == 'Z', "last page access error");

	// all page
	memset(pch, '0', len);
	return NULL;
}
