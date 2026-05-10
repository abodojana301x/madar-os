#include "udp.h"
#include "ip.h"
#include "../include/kprintf.h"

int udp_socket(int domain, int type, int protocol) {
    (void)domain;
    (void)type;
    (void)protocol;
    kprintf("udp_socket: OK (stub)\n");
    return 0;
}

int udp_sendto(int sockfd, const uint8_t* data, uint16_t len, const char* ip, uint16_t port) {
    (void)sockfd;
    (void)data;
    kprintf("udp_sendto: %s:%d len=%d (stub)\n", ip, port, len);
    return len;
}

int udp_recvfrom(int sockfd, uint8_t* buf, uint16_t len, char* ip, uint16_t* port) {
    (void)sockfd;
    (void)buf;
    (void)len;
    (void)ip;
    (void)port;
    kprintf("udp_recvfrom: waiting (stub)\n");
    return 0;
}
