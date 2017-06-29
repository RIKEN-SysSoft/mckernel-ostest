# Makefile COPYRIGHT FUJITSU LIMITED 2015-2016

CC?=gcc
RM=rm

default:
	${CC} ${LDFLAGS} entry-st.c -o lv07-st
	${CC} ${LDFLAGS} entry-pth.c -lpthread -o lv07-pth

clean:
	${RM} -f lv07-st lv07-pth
