#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "protocol.h"
#include "network.h"

int main(int argc, char **argv){
	
	if(argc < 6)
	{
		print_usage(argv[0]);
		return 1;
	}

	char *mode = argv[1];
	char *ip;
	uint16_t port;
	uint16_t address;
	uint16_t value;

	if(parse_arguments(argv, mode, &ip, &port, &address, &value) == -1){
		return 1;
	}

	int sockfd;
	if((sockfd = connect_to_server(ip, port)) == -1){
		return 1;
	}

	if(strcmp(mode, "read") == 0){
		if(send_command(address, value, 3, sockfd) == -1){
			close(sockfd);
			return 1;
		}	
	}
	else if(strcmp(mode, "write") == 0){
		if(send_command(address, value, 6, sockfd) == -1){
			close(sockfd);
			return 1;
		}
	}

	close(sockfd);
	return 0;
}