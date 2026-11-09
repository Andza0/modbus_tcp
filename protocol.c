#include "protocol.h"

int build_request(struct modbus_request config, uint8_t *req)
{
	uint8_t data[12] = {
		config.transaction_id >> 8, config.transaction_id,
		config.protocol_id >> 8,	config.protocol_id,
		config.length >> 8,		 	config.length,
		config.unit_id, 
		config.function_code,
		config.start_address >> 8,	config.start_address,
		config.count >> 8,		    config.count
	};

	memcpy(req, data, sizeof(data));
	return 0;
}

int validate_response(uint8_t resp_header[], uint8_t resp[], uint8_t req[])
{
	uint16_t recv_trans_id = (resp_header[0] << 8) | resp_header[1];
	uint16_t recv_prot_id = (resp_header[2] << 8) | resp_header[3];
	uint8_t recv_unit_id = resp_header[6];
	uint8_t recv_func_code = resp_header[7];

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

	if(recv_func_code == 0x83){
		uint8_t err = resp[0];
		print_err(err);
		return -1;
	}

	if(recv_func_code != req[7]) //function code
	{
		fprintf(stderr, "Received function code does not match\n");
		return -1;
	}

	if(resp[0] != (2 * (req[10] << 8 | req[11]))){  // count
		fprintf(stderr, "Byte count is lesser than required\n");
		return -1;
	}

	return 0;
}