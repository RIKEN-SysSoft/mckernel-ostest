/* 2node_recv.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 256

/* create server & packet recieve */
int main(int argc, char *argv[])
{
	/* portNUM, socket , param */
	unsigned short port = 9876;
	int srcSocket; 
	int dstSocket;
	struct sockaddr_in srcAddr;
	struct sockaddr_in dstAddr;
	int dstAddrSize = sizeof(dstAddr);
	int numrcv;
	int status;
	char buffer[BUFFER_SIZE];

	/* recieve counter */
	int recv_count = 0;

	/* setting struct sockaddr_in */
	memset(&srcAddr, 0, sizeof(srcAddr));
	srcAddr.sin_port = htons(port);
	srcAddr.sin_family = AF_INET;
	srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* socket create */
	srcSocket = socket(AF_INET, SOCK_STREAM, 0);

	/* socket bind */
	bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));

	/* connect permission */
	listen(srcSocket, 1);

	/* connection */
	printf("[server] Waiting for connection ...\n");
	dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
	printf("[server] Connected from %s\n", inet_ntoa(dstAddr.sin_addr));

	/* packet recieve */
	while(1) {
		numrcv = recv(dstSocket, buffer, BUFFER_SIZE, 0);
		if(numrcv == 0 || numrcv == -1) {
			if (numrcv == -1) {
				printf("[server] recv() failed. error code: %d\n", errno);
			}
			status = close(dstSocket);
			if (status != 0) {
				printf("[server] close() failed. error code: %d\n", errno);
			}
			break;
		}
		printf("[server] received[%d]: %s\n", recv_count, buffer);
		recv_count++;
	}
	return 0;
}
