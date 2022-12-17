//
// Created by mason on 2022/4/23.
//
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "utils.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		error(1, 0, "usage: udpclient <IPaddress>");
	}

	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	socklen_t server_len = sizeof(server_addr);

	struct sockaddr *reply_addr;
	reply_addr = malloc(server_len);

	char send_line[MAX_LINE], recv_line[MAX_LINE + 1];
	socklen_t len;
	int n;

	while (fgets(send_line, MAX_LINE, stdin) != NULL) {
		int i = strlen(send_line);
		if (send_line[i - 1] == '\n') {
			send_line[i - 1] = 0;
		}

		printf("now sending %s\n", send_line);
		size_t rt = sendto(socket_fd, send_line, strlen(send_line), 0,
						   (struct sockaddr *)&server_addr, server_len);
		if (rt < 0) {
			error(1, errno, "send failed ");
		}
		printf("send bytes: %zu \n", rt);

		struct timeval timout = {5, 0};
		if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timout,
					   sizeof(timout)) < 0) {
			error(1, 0, "setsockopt failed\n");
		}

		len = 0;
		n = recvfrom(socket_fd, recv_line, MAX_LINE, 0, reply_addr, &len);
		if (n < 0) {
			error(1, errno, "recvfrom failed");
		}

		recv_line[n] = 0;
		fputs(recv_line, stdout);
		fputs("\n", stdout);
	}

	return EXIT_SUCCESS;
}