/* setfsgid.c COPYRIGHT FUJITSU LIMITED 2015 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

#define FREAD_BUF_SIZE 1024
#define FNAME_SIZE 256

static struct setfsgid_args param;
void *setfsgid_parse_args(int argc, char **argv)
{
	int opt;
	memset(&param, 0, sizeof(param));

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			param.fsgid = atoi(optarg);
			break;
		default:
			return NULL;
		}
	}
	param.argv = argv + optind;
	param.argc = argc - optind;

	if (param.fsgid == -1 || param.fsgid == 0) {
		fprintf(stderr, "mandatory parameter '-f <fsgid>'\n");
	}
	return &param;
}

int check_procfs_status_gid(gid_t set_fsgid)
{
	char fname[FNAME_SIZE] = "";
	char buf[FREAD_BUF_SIZE] = "";
	FILE *fp = NULL;
	gid_t p_fsgid = 0;

	snprintf(fname, sizeof(fname), "/proc/%d/status", getpid());

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error.\n");
		goto err;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strstr(buf, "Gid:")) {
			char *p = strrchr(buf, '\t');

			printf("%s", buf);
			if (p != NULL) {
				p++;
				p_fsgid = atoi(p);
			}
			break;
		}
	}

	if (fclose(fp)) {
		fprintf(stderr, "fclose error.\n");
		goto err;
	}
	printf("cmp %d == %d ? \n", set_fsgid, p_fsgid);
	return (set_fsgid == p_fsgid);
err:
	return 0;
}
