#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_usage(char *program);

int main(int argc, char **argv){
	
	if(argc != 5){
		print_usage(argv[0]);
		return 1;
	}

	char *ip = argv[1];
	char *end;

	long port = strtol(argv[2], &end, 10);
	if((end == argv[2] || *end != '\0') || (port < 1 || port > 65535)){
		fprintf(stderr, "Invalid port\n");
		print_usage(argv[0]);
		return 1;
	}

	long address = strtol(argv[3], &end, 10);
	if((end == argv[3] || *end != '\0') || (address < 0 || address > 65535)){
		fprintf(stderr, "Invalid address\n");
		print_usage(argv[0]);
		return 1;
	}

	long count = strtol(argv[4], &end, 10);
	if((end == argv[4] || *end != '\0') || (count < 1 || count > 125)){
		fprintf(stderr, "Invalid count\n");
		print_usage(argv[0]);
		return 1;
	}


	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket");
		return 1;
	}
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(inet_pton(AF_INET, ip, &addr.sin_addr) != 1){
		fprintf(stderr, "Invalid ip address\n");
		close(sockfd);
		return 1;
	}

	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("connect");
		close(sockfd);
		return 1;
	}

	close(sockfd);

	return 0;
}

void print_usage(char *program)
{
	fprintf(stderr, "Usage: %s <ip> <port> <address> <count>\n", program);
}
