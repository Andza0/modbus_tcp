#include "protocol.h"

int validate_response(uint8_t MBAP_response[], uint8_t resp[], uint8_t req[])
{
	uint16_t recv_trans_id = (MBAP_response[0] << 8) | MBAP_response[1];
	uint16_t recv_prot_id = (MBAP_response[2] << 8) | MBAP_response[3];
	uint8_t recv_unit_id = MBAP_response[6];
	uint8_t recv_func_code = MBAP_response[7];

	if(recv_unit_id != req[6]){
		fprintf(stderr, "Unit IDs don't match\n");
		return -1;
	}

	if(recv_trans_id != (req[0] << 8 | req[1])){ //transaction ID
		fprintf(stderr, "Received transaction ID does not match\n");
		return -1;
	}

	if(recv_prot_id != 0){
		fprintf(stderr, "Protocol ID does not equal 0\n");
		return -1;
	}

	if(recv_func_code == (req[7] | 0x80)){
		uint8_t err = resp[0];
		print_err(err);
		return -1;
	}

	if(recv_func_code != req[7]) //function code
	{
		fprintf(stderr, "Received function code does not match\n");
		return -1;
	}

	if(req[7] == 6){
		if((resp[0] << 8 | resp[1]) != (req[8] << 8 | req[9])){
			fprintf(stderr, "Adressess don't match\n");
			return -1;
		}

		if((resp[2] << 8 | resp[3]) != (req[10] << 8 | req[11]))
		{
			fprintf(stderr, "Values don't match\n");
			return -1;
		}

		return 0;
	}

	if(resp[0] != (2 * (req[10] << 8 | req[11]))){  // count
		fprintf(stderr, "Byte count is lesser than required\n");
		return -1;
	}

	return 0;
}

int read_registers(uint16_t start_address, uint8_t count, int sockfd)
{
	struct modbus_request config = {
		.transaction_id = 1,
		.protocol_id = 0,
		.length = 6,
		.unit_id = 1,
		.function_code = 3,
		.address = start_address,
		.value = count
	};

	uint8_t read_req[12] = {
		config.transaction_id >> 8, config.transaction_id,
		config.protocol_id >> 8,	config.protocol_id,
		config.length >> 8,		 	config.length,
		config.unit_id, 
		config.function_code,
		config.address >> 8,		config.address,
		config.value >> 8,		    config.value
	};

	print_hex("Request", read_req, 12);

	if(send_req(read_req, 12, sockfd) == -1){
		return -1;
	}

	uint8_t MBAP_response[8]; //read MBAP + function code
	if(recv_resp(MBAP_response, 8, sockfd) == -1){
		return -1;
	}

	uint16_t recv_len = (MBAP_response[4] << 8) | MBAP_response[5];
	int expected_len = 3 + 2 * count;

	print_hex("Header", MBAP_response, 8);
	if(recv_len != expected_len && !(MBAP_response[7] == 0x83 && recv_len == 3)){
		fprintf(stderr, "Incorrect length reading\n");
		return -1;
	}

	uint8_t resp[recv_len - 2];
	if(recv_resp(resp, recv_len - 2, sockfd) == -1){
		return -1;
	}

	int is_valid = validate_response(MBAP_response, resp, read_req);
	if(is_valid != 0){
		return -1;
	}

	print_hex("Response", resp, recv_len - 2);
	print_registers(resp, count, start_address);

	return 0;
}

int write_register(uint16_t address, uint16_t write_val, int sockfd)
{
	struct modbus_request config = {
		.transaction_id = 1,
		.protocol_id = 0,
		.length = 6,
		.unit_id = 1,
		.function_code = 6,
		.address = address,
		.value = write_val
	};

	uint8_t write_req[12] = {
		config.transaction_id >> 8, config.transaction_id,
		config.protocol_id >> 8,	config.protocol_id,
		config.length >> 8, 		config.length,
		config.unit_id, 
		config.function_code,
		config.address >> 8,	config.address,
		config.value >> 8,		config.value
	};

	if(send_req(write_req, 12, sockfd) == -1){
		return -1;
	}

	uint8_t MBAP_response[8]; //read MBAP + function code
	if(recv_resp(MBAP_response, 8, sockfd) == -1){
		return -1;
	}

	uint16_t recv_len = (MBAP_response[4] << 8) | MBAP_response[5];
	int expected_len = 6;

	print_hex("Header", MBAP_response, 8);
	if(recv_len != expected_len && !(MBAP_response[7] == 0x86 && recv_len == 3)){
		fprintf(stderr, "Incorrect length reading\n");
		return -1;
	}

	uint8_t resp[recv_len - 2];
	if(recv_resp(resp, recv_len - 2, sockfd) == -1){
		return -1;
	}

	int is_valid = validate_response(MBAP_response, resp, write_req);
	if(is_valid != 0){
		return -1;
	}

	print_hex("Response", resp, recv_len - 2);
	printf("Register %u: %u\n", (resp[0] << 8 | resp[1]), (resp[2] << 8 | resp[3]));
	
	return 0;
}