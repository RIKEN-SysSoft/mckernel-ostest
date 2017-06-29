# Makefile COPYRIGHT FUJITSU LIMITED 2015-2016
CC?=gcc
RM=rm

default:
	${CC} ${LDFLAGS} single_node.c -lc -lpthread -o single_node
	${CC} ${LDFLAGS} 2node_send.c -lc -lpthread -o 2node_send
	${CC} ${LDFLAGS} 2node_recv.c -lc -lpthread -o 2node_recv

clean:
	${RM} -f single_node 2node_send 2node_recv
