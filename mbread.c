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

	uint8_t count;
	uint16_t write_val;

	if(parse_arguments(argv, mode, &ip, &port, &address, &count, &write_val) == -1){
		return 1;
	}

	int sockfd;
	if((sockfd = connect_to_server(ip, port)) == -1){
		return 1;
	}

	if(strcmp(mode, "read") == 0){
		if(read_registers(address, count, sockfd) == -1){
			close(sockfd);
			return 1;
		}	
	}
	else if(strcmp(mode, "write") == 0){
		if(write_register(address, write_val, sockfd) == -1){
			close(sockfd);
			return 1;
		}
	}

	close(sockfd);
	return 0;
}