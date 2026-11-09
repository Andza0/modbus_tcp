#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>


int send_req(uint8_t req[], size_t sz, int sockfd);
int recv_resp(uint8_t resp[], size_t sz, int sockfd);
int connect_to_server(char *ip, uint16_t port);

#endif