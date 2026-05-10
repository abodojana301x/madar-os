#ifndef UDP_H
#define UDP_H

#include "net_stack.h"
#include "../include/types.h"

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
    uint8_t data[1500];
} udp_header_t;

int udp_socket(int domain, int type, int protocol);
int udp_sendto(int sockfd, const uint8_t* data, uint16_t len, const char* ip, uint16_t port);
int udp_recvfrom(int sockfd, uint8_t* buf, uint16_t len, char* ip, uint16_t* port);

#endif

