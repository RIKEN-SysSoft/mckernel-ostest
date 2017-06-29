/* main.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

static char* bss_path = NULL;
static char* data_path = (char*)0xFF;

void usage(const char* name)
{
	fprintf(stderr,
			"Usage: %s r <path>\n"
			"       %s w <path> <string>\n",
			name, name);
}

int write_to_file(const char* path, const char* string)
{
	int ret = -1;
	int fd = -1;
    char *data = (char*)-1;
    struct stat sbuf;
	
    if ((fd = open(path, O_RDWR)) == -1) {
        perror("open");
        goto out;
    }
	
    if (stat(path, &sbuf) == -1) {
        perror("stat");
        goto out;
    }

	if (sbuf.st_size <= strlen(string)) {
		fprintf(stderr, "string size error.\n");
        goto out;
	}

    data = (char *)mmap((caddr_t)0, sbuf.st_size,
						PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == (char *)-1) {
        perror("mmap");
        goto out;
    }

	strcpy(data, string);	
	fprintf(stdout, "%s", data);
	ret = 0;
out:
	if (data != (char*)-1) {
		if (munmap(data, sbuf.st_size) == -1) {
			perror("munmap");
		}
	}

	if (fd != -1) {
		close(fd);
		fd = -1;
	}
	return ret;
}

int read_from_file(const char* path)
{
	int ret = -1;
	int fd = -1;
    char *data = (char*)-1;
    struct stat sbuf;
	
    if ((fd = open(path, O_RDONLY)) == -1) {
        perror("open");
        goto out;
    }
	
    if (stat(path, &sbuf) == -1) {
        perror("stat");
        goto out;
    }

    data = (char *)mmap((caddr_t)0, sbuf.st_size,
						PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == (char *)-1) {
        perror("mmap");
        goto out;
    }
	
	fprintf(stdout, "%s", data);
	ret = 0;
out:
	if (data != (char*)-1) {
		munmap(data, sbuf.st_size);
	}

	if (fd != -1) {
		close(fd);
		fd = -1;
	}
	return ret;
}

int main(int argc, char *argv[])
{
	int ret;
	const char* mode = NULL;
	const char* path = NULL;
	const char* string = NULL;

    if (argc < 3) {
		usage(argv[0]);
        return -1;
    }
	mode = argv[1];
	path = argv[2];

	if (mode[0] == 'r' && 3 <= argc) {
		ret = read_from_file(path);
	}
	else if (mode[0] == 'w' && 3 <= argc) {
		string = argv[3];
		ret = write_to_file(path, string);
	}
	else {
		usage(argv[0]);
        return -1;
	}

	return ret;
}
