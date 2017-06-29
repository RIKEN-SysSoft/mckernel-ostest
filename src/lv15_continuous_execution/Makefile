# Makefile COPYRIGHT FUJITSU LIMITED 2015-2016
CC?=gcc
ARCH?=x86
RM=rm
CFLAGS+=-Iarch/${ARCH}/include

default:
	${CC} ${LDFLAGS} ${CFLAGS} lv15_kill_process.c -o lv15-kill
	${CC} ${LDFLAGS} lv15_map_anon.c -o lv15-manon
	${CC} ${LDFLAGS} lv15_map_file.c -o lv15-mfile

clean:
	${RM} -f lv15-kill lv15-manon lv15-mfile
