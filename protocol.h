#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

int build_request(uint16_t address, uint16_t count, uint8_t *req);
int validate_response(uint8_t resp_header[], uint8_t resp[], uint8_t req[]);