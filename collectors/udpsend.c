#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

int udpsend(const char *host, const int port, const char *msg) {
	int sock, connect_ret, read_ret, send_ret, i;
	char c;
	struct sockaddr_in server;
	FILE *si;

	if(port <=0 || port >= 65000)
		return 1;
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		return 1;
	server.sin_addr.s_addr = inet_addr(host);
	if(server.sin_addr.s_addr == -1)
		return 1;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	connect_ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if(connect_ret == -1) 
		return 1;
	send(sock, (const void*)msg, (size_t)strlen(msg), 0);
	if(send_ret == -1)
		return 1;
	return 0;
}

