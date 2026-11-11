#include "network.h"



int send_req(uint8_t req[], size_t sz, int sockfd)
{
	struct timeval timeout = {
		.tv_sec = 2,
		.tv_usec = 0
	};
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	size_t sent = 0;
	while(sent < sz){		
		ssize_t n = send(sockfd, req + sent, sz - sent, 0);
		if(n == -1){
			perror("send");
			return -1;
		}
		if(n == 0){
			fprintf(stderr, "No bytes were sent\n");
			return -1;
		}
		sent = sent + n;
	}

	return sent;
}

int recv_resp(uint8_t resp[], size_t sz, int sockfd)
{
	struct timeval timeout = {
		.tv_sec = 2,
		.tv_usec = 0
	};
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	size_t received = 0;
	while(received < sz){
		ssize_t n = recv(sockfd, resp + received, sz - received, 0);
		if(n == -1){
			perror("recv");
			return -1;
		}
		if(n == 0){
			fprintf(stderr, "Connection closed\n");
			return -1;
		}
		received = received + n;
	}
	return received;
}

int connect_to_server(char *ip, uint16_t port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(inet_pton(AF_INET, ip, &addr.sin_addr) != 1){
		fprintf(stderr, "Invalid ip address\n");
		close(sockfd);
		return -1;
	}

	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("connect");
		close(sockfd);
		return -1;
	}

	return sockfd;
}
