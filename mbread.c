#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_usage(char *program);
int send_req(uint8_t req[], size_t sz, int sockfd);
int recv_resp(uint8_t resp[], size_t sz, int sockfd);
void print_err(uint8_t err);

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


	uint16_t transaction_id = 1;
	uint16_t protocol_id = 0;
	uint16_t length = 6;
	uint8_t unit_id = 1;
	uint8_t function_code = 3;

	uint8_t req[12] = {
		transaction_id >> 8, transaction_id, //Transactio ID
		protocol_id >> 8,	 protocol_id, 	//Protocol ID
		length >> 8,		 length, 		//Length
		unit_id, 							//Unit ID
		function_code,	    				//Function code
		address >> 8,		 address, 		//Start address
		count >> 8,		     count  		//Count
	};

	printf("Request: ");
	for(int i = 0; i < 12; i++){
		printf("%02X ", req[i]);
	}
	printf("\n");

	if(send_req(req, 12, sockfd) == -1){
		close(sockfd);
		return 1;
	}

	uint8_t resp_header[8];
	if(recv_resp(resp_header, 8, sockfd) == -1){
		close(sockfd);
		return 1;
	}


	uint16_t recv_trans_id = (resp_header[0] << 8) | resp_header[1];
	uint16_t recv_prot_id = (resp_header[2] << 8) | resp_header[3];
	uint16_t recv_len = (resp_header[4] << 8) | resp_header[5];
	uint8_t recv_func_code = resp_header[7];
	
	uint8_t resp[recv_len - 2];
	if(recv_resp(resp, recv_len - 2, sockfd) == -1){
		close(sockfd);
		return 1;
	}
	close(sockfd);

	if(recv_trans_id != transaction_id){
		fprintf(stderr, "Received transaction ID does not match\n");
		return 1;
	}

	if(recv_prot_id != 0){
		fprintf(stderr, "Protocol ID does not equal 0\n");
		return 1;
	}

	if(recv_func_code == 0x83){
		uint8_t err = resp[0];
		print_err(err);
		return 1;
	}

	if(recv_func_code != function_code)
	{
		fprintf(stderr, "Received function code does not match\n");
		return 1;
	}

	if(resp[0] != (2 * count)){
		fprintf(stderr, "Byte count is lesser than required\n");
		return 1;
	}

	printf("Response: ");
	for(int i = 0; i < recv_len - 2; i++){
		printf("%02X ", resp[i]);
	}
	printf("\n");

	for(int i = 1; i <= count; i++){
		uint16_t value = resp[i + (i - 1)] << 8 | resp[i * 2];
		printf("Register %lu: %u\n", address + i, value);
	}


	return 0;
}

void print_usage(char *program)
{
	fprintf(stderr, "Usage: %s <ip> <port> <address> <count>\n", program);
}

int send_req(uint8_t req[], size_t sz, int sockfd)
{
	size_t sent = 0;
	while(sent < sz){		
		int n = send(sockfd, req + sent, sz - sent, 0);
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
	size_t received = 0;
	while(received < sz){
		int n = recv(sockfd, resp + received, sz - received, 0);
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

void print_err(uint8_t err)
{
	switch(err)
	{
		case 1:
			fprintf(stderr, "ERROR 1: illegal function\n");
			break;
		case 2:
			fprintf(stderr, "ERROR 2: illegal data address\n");
			break;
		case 3:
			fprintf(stderr, "ERROR 3: illegal data value\n");
			break;
		case 4:
			fprintf(stderr, "ERROR 4: server device failure\n");
			break;
		default:
			fprintf(stderr, "UNKNOWN ERROR\n");
	}
}
