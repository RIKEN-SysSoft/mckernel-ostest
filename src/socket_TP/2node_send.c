/* 2node_send.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define CONNECT_RETRY 5
#define SEND_COUNT 10

void print_usage(void)
{
	printf("[Usage]\n");
	printf("$./2node_send ipaddr\n");
	printf("    ipaddr: destination IP address(v4)\n");
}

/* create client & packet send */
int main(int argc, char *argv[])
{
	/* IPaddress, portNUM, socket, param */
	char destination[80];
	unsigned short port = 9876;
	int dstSocket;
	struct sockaddr_in dstAddr;
	int status;
	int i, numsnt, ret = 0, retry_count = 0;
	char *toSendText = "This is a test";

	if (argc < 2) {
		print_usage();
		return 0;
	}

	strcpy(destination, argv[1]);
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
			return -1;
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
	return 0;
}

