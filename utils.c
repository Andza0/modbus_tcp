#include "utils.h"

int parse_arguments(char **argv, char *mode, char **ip, uint16_t *port, uint16_t *address, uint8_t *count, uint16_t *write_val)
{
	char *end;

	if(strcmp(mode, "read") == 0){
		long address_long = strtol(argv[4], &end, 10);
		if((end == argv[4] || *end != '\0') || (address_long < 0 || address_long > 65535)){
			fprintf(stderr, "Invalid address\n");
			print_usage(argv[0]);
			return -1;
		}
		*address = address_long;

		long count_long = strtol(argv[5], &end, 10);
		if((end == argv[5] || *end != '\0') || (count_long < 1 || count_long > 125)){
			fprintf(stderr, "Invalid count\n");
			print_usage(argv[0]);
			return -1;
		}
		*count = count_long;
	}
	else if(strcmp(mode, "write") == 0){
		long address_long = strtol(argv[4], &end, 10);
		if((end == argv[4] || *end != '\0') || (address_long < 0 || address_long > 10)){
			fprintf(stderr, "Invalid address\n");
			print_usage(argv[0]);
			return -1;
		}
		*address = address_long;

		long write_val_long = strtol(argv[5], &end, 10);
		if((end == argv[5] || *end != '\0') || (write_val_long < 0 || write_val_long > UINT16_MAX)){
			fprintf(stderr, "Invalid value\n");
			print_usage(argv[0]);
			return -1;
		}
		*write_val = write_val_long;
	}
	else{;

		print_usage(argv[0]);
		return -1;
	}

	*ip = argv[2];

	long port_long = strtol(argv[3], &end, 10);
	if((end == argv[3] || *end != '\0') || (port_long < 1 || port_long > 65535)){
		fprintf(stderr, "Invalid port\n");
		print_usage(argv[0]);
		return -1;
	}
	*port = port_long;

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
			fprintf(stderr, "ERROR 3: illegal data value\n");
			break;
		case 4:
			fprintf(stderr, "ERROR 4: server device failure\n");
			break;
		default:
			fprintf(stderr, "UNKNOWN ERROR\n");
	}
}

void print_registers(uint8_t resp[], uint8_t count, uint16_t address)
{
	for(int i = 1; (uint)i <= count; i++){
		uint16_t value = resp[i + (i - 1)] << 8 | resp[i * 2];
		printf("Register %u: %u\n", address + (i - 1), value);
	}

}

void print_hex(char *header, uint8_t arr[], size_t sz)
{
	printf("%s: ", header);
	for(int i = 0; (uint)i < sz; i++){
		printf("%02X ", arr[i]);
	}
	printf("\n");
}

void print_usage(char *program)
{
	fprintf(stderr, "Usage: %s <mode> <ip> <port> <address> <count>/<value>\n", program);
}