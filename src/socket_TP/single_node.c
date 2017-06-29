/* single_node.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
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
void* socket_server_create(void* args)
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
}

#define CONNECT_RETRY 5
#define SEND_COUNT 10

/* create client & packet send */
void socket_client_create() {
	/* IPaddress, portNUM, socket, param */
	char destination[80] = "127.0.0.1";	/* connect to localhost */
	unsigned short port = 9876;
	int dstSocket;
	struct sockaddr_in dstAddr;
	int status;
	int i, numsnt, ret = 0, retry_count = 0;
	char *toSendText = "This is a test";

	printf("[client] Connect to %s\n", destination);

	/* setting struct sockaddr_in */
	memset(&dstAddr, 0, sizeof(dstAddr));
	dstAddr.sin_port = htons(port);
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_addr.s_addr = inet_addr(destination);

	/* create socket */
	dstSocket = socket(AF_INET, SOCK_STREAM, 0);

	/* connect */
	printf("[client] Trying to connect to %s: \n", destination);
	while (connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr))) {
		printf("[client] connect() failed. error code: %d\n", errno);
		if (CONNECT_RETRY < retry_count) {
			printf("[client] connect() retry count over.\n");
			return;
		}
		sleep(1);
		printf("[client] retry connect()\n");
		retry_count++;
	}

	/* packet send */
	for(i = 0; i < SEND_COUNT; i++) {
		printf("[client] sending...[%d]\n", i);
		ret = send(dstSocket, toSendText, strlen(toSendText)+1, 0);
		if (ret == -1) {
			printf("[client] send() failed. error code: %d\n", errno);
		}
		sleep(1);
	}
	/* socket close */
	close(dstSocket);
	return;
}

int main(int argc, char *argv[]) {
	pthread_t thread;

	/* create child thread(receiver) */
	pthread_create(&thread, NULL, socket_server_create, NULL);

	/* parent thread is sender */
	socket_client_create();

	/* child thread wait */
	pthread_join(thread, NULL);

	return 0;
}
