#include "utils.h"

int parse_arguments(char **argv, char *mode, char **ip, uint16_t *port, uint16_t *address, uint16_t *value)
{
	char *end;

	if(strcmp(mode, "read") == 0){
		long read_count = strtol(argv[5], &end, 10);
		if((end == argv[5] || *end != '\0') || (read_count < 1 || read_count > 125)){
			fprintf(stderr, "Invalid read_count\n");
			print_usage(argv[0]);
			return -1;
		}
		*value = read_count;
	}
	else if(strcmp(mode, "write") == 0){
		long write_val = strtol(argv[5], &end, 10);
		if((end == argv[5] || *end != '\0') || (write_val < 0 || write_val > UINT16_MAX)){
			fprintf(stderr, "Invalid read_count\n");
			print_usage(argv[0]);
			return -1;
		}
		*value = write_val;
	}
	else{;

		print_usage(argv[0]);
		return -1;
	}

	*ip = argv[2];

	long address_val = strtol(argv[4], &end, 10);
	if((end == argv[4] || *end != '\0') || (address_val < 0 || address_val > 65535)){
		fprintf(stderr, "Invalid address\n");
		print_usage(argv[0]);
		return -1;
	}
	*address = address_val;

	long port_val = strtol(argv[3], &end, 10);
	if((end == argv[3] || *end != '\0') || (port_val < 1 || port_val > 65535)){
		fprintf(stderr, "Invalid port\n");
		print_usage(argv[0]);
		return -1;
	}
	*port = port_val;

	return 0;
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
			fprintf(stderr, "ERROR 3: illegal data read_count\n");
			break;
		case 4:
			fprintf(stderr, "ERROR 4: server device failure\n");
			break;
		default:
			fprintf(stderr, "UNKNOWN ERROR\n");
	}
}

void print_registers(uint8_t resp[], uint16_t address, uint16_t count, char *mode)
{
	uint16_t value = 0;
	for(int i = 1; (unsigned int)i <= count; i++){
		if(strcmp(mode, "write") == 0){
			value = resp[i * 2] << 8 | resp[(i * 2) + 1];
		}
		else if(strcmp(mode, "read") == 0){
			value = resp[(i * 2) - 1] << 8 | resp[i * 2];
		}
		printf("Register %u: %u\n", address + (i - 1), value);
	}
}

void print_hex(char *header, uint8_t arr[], size_t sz)
{
	printf("%s: ", header);
	for(int i = 0; (unsigned int)i < sz; i++){
		printf("%02X ", arr[i]);
	}
	printf("\n");
}

void print_usage(char *program)
{
	fprintf(stderr, "Usage: %s <mode> <ip> <port> <address> <read_count>/<read_count>\n", program);
}

//i=0 i=1   i=2	  i=3   i=4   i=5   i=6   i=7   i=8   i=9
//14 00 0A 00 01 00 01 00 01 00 01 00 37 00 00 00 01 00 01 00 01
//0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20

//0  1  2  3
//00 05 00 05
//0  1  2  3