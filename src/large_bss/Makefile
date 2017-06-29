# Makefile COPYRIGHT FUJITSU LIMITED 2015
CC?=gcc
ARCH?=x86
RM=rm
CFLAGS += -g
CFLAGS += -O0
CFLAGS += -Wall

default:
	${CC} ${LDFLAGS} ${CFLAGS} large_bss.c -o large_bss

clean:
	${RM} -f large_bss
