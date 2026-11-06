#include "protocol.h"

int build_request(uint16_t address, uint16_t count, uint8_t *req)
{
	uint16_t transaction_id = 1;
	uint16_t protocol_id = 0;
	uint16_t length = 6;
	uint8_t unit_id = 1;
	uint8_t function_code = 3;

	uint8_t data[12] = {
		transaction_id >> 8, transaction_id, //Transaction ID
		protocol_id >> 8,	 protocol_id, 	//Protocol ID
		length >> 8,		 length, 		//Length
		unit_id, 							//Unit ID
		function_code,	    				//Function code
		address >> 8,		 address, 		//Start address
		count >> 8,		     count  		//Count
	};

	memcpy(req, data, sizeof(data));
	return 0;
}

int validate_response(uint8_t resp_header[], uint8_t resp[], uint8_t req[])
{
	uint16_t recv_trans_id = (resp_header[0] << 8) | resp_header[1];
	uint16_t recv_prot_id = (resp_header[2] << 8) | resp_header[3];
	uint8_t recv_func_code = resp_header[7];

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