#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "network.h"

struct modbus_request
{
    uint16_t transaction_id;
    uint16_t protocol_id;
    uint16_t length;
    uint8_t unit_id;
    uint8_t function_code;
    uint16_t address;
    uint16_t value;
};


int validate_response(uint8_t resp_header[], uint8_t resp[], uint8_t req[]);
int send_command(uint16_t address, uint16_t value, uint8_t function_code, int sockfd);
void build_request(struct modbus_request config, uint8_t *req);

#endif