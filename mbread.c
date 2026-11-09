#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "protocol.h"
#include "network.h"

int main(int argc, char **argv){
	
	char *ip;
	uint16_t port;
	uint16_t address;
	uint8_t count;

	if(parse_arguments(argc, argv, &ip, &port, &address, &count) == -1){
		return 1;
	}

	int sockfd;
	if((sockfd = connect_to_server(ip, port)) == -1){
		return 1;
	}

	struct modbus_request config = {
		.transaction_id = 1,
		.protocol_id = 0,
		.length = 6,
		.unit_id = 1,
		.function_code = 3,
		.start_address = address,
		.count = count
	};

	uint8_t req[12];
	if(build_request(config, req) != 0)
	{
		fprintf(stderr, "Request array too small\n");
		return 1;
	}

	print_hex("Request", req, 12);

	if(send_req(req, 12, sockfd) == -1){
		close(sockfd);
		return 1;
	}

	uint8_t resp_header[8];
	if(recv_resp(resp_header, 8, sockfd) == -1){
		close(sockfd);
		return 1;
	}
	uint16_t recv_len = (resp_header[4] << 8) | resp_header[5];
	int expected_len = 3 + 2 * count;
	if(recv_len != expected_len && !(resp_header[7] == 0x83 && recv_len == 3)){
		fprintf(stderr, "Incorrect length reading\n");
		close(sockfd);
		return 1;
	}

	uint8_t resp[recv_len - 2];
	if(recv_resp(resp, recv_len - 2, sockfd) == -1){
		close(sockfd);
		return 1;
	}
	close(sockfd);

	int is_valid = validate_response(resp_header, resp, req);
	if(is_valid != 0){
		return 1;
	}

	print_hex("Response", resp, recv_len - 2);
	print_registers(resp, count, address);

	return 0;
}