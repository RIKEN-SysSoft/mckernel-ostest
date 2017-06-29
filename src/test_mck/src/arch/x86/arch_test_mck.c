/* arch_test_mck.c COPYRIGHT FUJITSU LIMITED 2015 */

/* auxv */
char *auxv_name[] = { "AT_NULL",     "AT_IGNORE", "AT_EXECFD", "AT_PHDR",   "AT_PHENT", 	/* 0-4 */
		      "AT_PHNUM",    "AT_PAGESZ", "AT_BASE",   "AT_FLAGS",  "AT_ENTRY",		/* 5-9 */
		      "AT_NOTELF",   "AT_UID",    "AT_EUID",   "AT_GID",    "AT_EGID", 		/* 10-14 */
		      "AT_PLATFORM", "AT_HWCAP",  "AT_CLKTCK", "",          "", 		/* 15-19 */
		      "",            "",          "",          "AT_SECURE", "AT_BASE_PLATFORM", /* 20-24 */
		      "AT_RANDOM",   "",          "",          "",          "", 		/* 25-29 */
		      "",            "AT_EXECFN", "AT_SYSINFO", "AT_SYSINFO_EHDR"};		/* 30-33 */
