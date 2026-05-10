#ifndef TCP_H
#define TCP_H

#include "net_stack.h"
#include "../include/types.h"

#define TCP_PORT_HTTP 80
#define TCP_MAX_SEG 1460
#define TCP_WINDOW 65535

typedef enum {
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RCVD,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_TIME_WAIT
} tcp_state_t;

typedef struct {
    uint32_t seq;
    uint32_t ack;
    uint16_t window;
    uint8_t flags; // SYN, ACK, FIN, RST
    uint16_t checksum;
} tcp_header_t;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    tcp_header_t hdr;
    uint8_t data[TCP_MAX_SEG];
    uint16_t data_len;
} tcp_segment_t;

int tcp_socket(int domain, int type, int protocol);
int tcp_connect(int sockfd, const char* ip, uint16_t port);
int tcp_send(int sockfd, const uint8_t* data, uint16_t len);
int tcp_recv(int sockfd, uint8_t* buf, uint16_t len);
int tcp_close(int sockfd);

#endif

