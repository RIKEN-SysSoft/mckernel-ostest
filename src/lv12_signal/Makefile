# Makefile COPYRIGHT FUJITSU LIMITED 2015-2016

CC?=gcc
ARCH?=x86
RM=rm
CFLAGS+=-Iarch/${ARCH}/include

default:
	${CC} ${LDFLAGS} ${CFLAGS} entry-kill.c -o lv12-kill
	${CC} ${LDFLAGS} entry-kill-single.c -o lv12-kill-single
	${CC} ${LDFLAGS} entry-segv.c -o lv12-segv

clean:
	${RM} -f lv12-kill lv12-kill-single lv12-segv
