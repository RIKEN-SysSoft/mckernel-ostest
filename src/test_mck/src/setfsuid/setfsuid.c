/* setfsuid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

#define FREAD_BUF_SIZE 1024
#define FNAME_SIZE 256

static struct setfsuid_args param;
void *setfsuid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			param.fsuid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.fsuid == -1 || param.fsuid == 0) {
		fprintf(stderr, "mandatory parameter '-f <fsuid>'\n");
	}
	return &param;
}

int check_procfs_status_uid(uid_t set_fsuid)
{
	char fname[FNAME_SIZE] = "";
	char buf[FREAD_BUF_SIZE] = "";
	FILE *fp = NULL;
	uid_t p_fsuid = 0;

	snprintf(fname, sizeof(fname), "/proc/%d/status", getpid());

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error.\n");
		goto err;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strstr(buf, "Uid:")) {
			char *p = strrchr(buf, '\t');

			printf("%s", buf);
			if (p != NULL) {
				p++;
				p_fsuid = atoi(p);
			}
			break;
		}
	}

	if (fclose(fp)) {
		fprintf(stderr, "fclose error.\n");
		goto err;
	}
	printf("cmp %d == %d ? \n", set_fsuid, p_fsuid);
	return (set_fsuid == p_fsuid);
err:
	return 0;
}
