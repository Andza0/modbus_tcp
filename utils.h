#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void print_hex(char *header, uint8_t arr[], size_t sz);
void print_registers(uint8_t resp[], uint16_t address, uint16_t count, char *mode);

int parse_arguments(char **argv, char *mode, char **ip, uint16_t *port, uint16_t *address, uint16_t *value);

void print_err(uint8_t err);
void print_usage(char *program);

#endif