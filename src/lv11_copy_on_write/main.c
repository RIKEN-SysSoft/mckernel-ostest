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

#define BUF_SIZE 1024

enum mode {
	MODE_NULL = 0,
	MODE_WRITE,
	MODE_READ,
};

struct args {
	int mode;
	int sync;
	int mmap_prot;
	int mmap_flags;
	char path[BUF_SIZE];
};

void usage(const char* name)
{
	char _name[BUF_SIZE] = {0};
	strncpy(_name, name, BUF_SIZE);
	_name[BUF_SIZE-1] = '\0';

	fprintf(stderr,
			"usage:\n"
			"  %s <mode> <prot_flags> <path>\n"
			"options:\n"
			"       mode: r    read\n"
			"             w    write\n"
			"             s    msync\n"
			" prot_flags: r    MAP_READ\n"
			"             w    MAP_WRITE\n"
			"             e    MAP_EXEC\n"
			"             s    MAP_SHARED\n"
			"             p    MAP_PRIVATE\n"
			"       path: file path\n",
			_name, _name);
}

int write_to_file(const char* path,
				  int prot, int flags,
				  int sync, const char* string)
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
						prot, flags, fd, 0);
    if (data == (char *)-1) {
        perror("mmap");
        goto out;
    }

	strcpy(data, string);	
	fprintf(stdout, "%s", data);

	if (sync) {
		if (msync(data, sbuf.st_size, MS_SYNC) == -1) {
			perror("msync");
			goto out;
		}
	}

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

int read_from_file(const char* path,
				   int prot, int flags,
				   int sync)
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
						prot, flags, fd, 0);
    if (data == (char *)-1) {
        perror("mmap");
        goto out;
    }
	
	fprintf(stdout, "%s", data);

	if (sync) {
		if (msync(data, sbuf.st_size, MS_SYNC) == -1) {
			perror("msync");
			goto out;
		}
	}

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

struct args parse_argument(int argc, char *argv[])
{
	struct args args = {
		.mode = MODE_NULL,
		.sync = 0,
		.mmap_prot = 0,
		.mmap_flags = 0,
		.path = {0},
	};
	const char *arg = NULL;
	int pos = 0;

    if (argc < 4) {
		goto fault;
    }
	// mode
	arg = argv[1];
	pos = 0;
	while (arg[pos] != '\0') {
		switch (arg[pos]) {
		case 'w':
			args.mode = MODE_WRITE;
			break;
		case 'r':
			args.mode = MODE_READ;
			break;
		case 's':
			args.sync = 1;
			break;
		default:
			goto fault;
		}
		pos++;
	}

	// mmap prot
	arg = argv[2];
	pos = 0;
	while (arg[pos] != '\0') {
		switch (arg[pos]) {
		case 'r':
			args.mmap_prot |= PROT_READ;
			break;
		case 'w':
			args.mmap_prot |= PROT_WRITE;
			break;
		case 'e':
			args.mmap_prot |= PROT_EXEC;
			break;
		case 's':
			args.mmap_flags |= MAP_SHARED;
			break;
		case 'p':
			args.mmap_flags |= MAP_PRIVATE;
			break;
		default:
			goto fault;
		}
		pos++;
	}

	// path
	strncpy(args.path, argv[3], BUF_SIZE);
	args.path[BUF_SIZE-1] = '\0';
	return args;
fault:
	args.mode = MODE_NULL;
	return args;
}

int main(int argc, char *argv[])
{
	int ret;
	struct args args;

	// parse argument
	args = parse_argument(argc, argv);

	if (args.mode == MODE_READ) {
		ret = read_from_file(args.path,
							 args.mmap_prot,
							 args.mmap_flags,
							 args.sync);
	}
	else if (args.mode == MODE_WRITE) {
		ret = write_to_file(args.path,
							args.mmap_prot,
							args.mmap_flags,
							args.sync,
							"mmap-write");
	}
	else {
		usage(argv[0]);
        ret = -1;
	}
	return ret;
}
