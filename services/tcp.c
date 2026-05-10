#include "tcp.h"
#include "ip.h"
#include "../include/kprintf.h"
#include "../include/spinlock.h"
#include "../include/string.h"

// Simple TCP connection table (max 4 for v0.1)
#define MAX_TCP_CONNS 4
static tcp_segment_t conns[MAX_TCP_CONNS];
static spinlock_t tcp_lock;
static int tcp_lock_ready;

static int find_free_conn() {
    for (int i = 0; i < MAX_TCP_CONNS; i++) {
        if (conns[i].hdr.seq == 0) return i;
    }
    return -1;
}

static uint32_t tcp_parse_ipv4(const char *ip_str) {
    uint32_t parts[4] = {0, 0, 0, 0};
    int part = 0;
    if (!ip_str) return 0;
    for (const char *p = ip_str; *p && part < 4; p++) {
        if (*p == '.') {
            part++;
        } else if (*p >= '0' && *p <= '9') {
            parts[part] = (parts[part] * 10U) + (uint32_t)(*p - '0');
        } else {
            return 0;
        }
    }
    return (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
}

int tcp_socket(int domain, int type, int protocol) {
    (void)domain;
    (void)type;
    (void)protocol;
    if (!tcp_lock_ready) {
        spinlock_init(&tcp_lock);
        tcp_lock_ready = 1;
    }
    int sockfd = find_free_conn();
    if (sockfd < 0) return -1;
    conns[sockfd].hdr.seq = 0x1000U + (uint32_t)sockfd;
    conns[sockfd].hdr.ack = 0;
    kprintf("tcp_socket: fd=%d\n", sockfd);
    return sockfd;
}

int tcp_connect(int sockfd, const char* ip_str, uint16_t port) {
    if (sockfd < 0 || sockfd >= MAX_TCP_CONNS) return -1;
    spinlock_acquire(&tcp_lock);
    
    // SYN packet
    conns[sockfd].src_port = 54321; // Ephemeral
    conns[sockfd].dst_port = port;
    conns[sockfd].hdr.flags = 0x02; // SYN
    conns[sockfd].data_len = 0;
    
    uint32_t dst_ip = tcp_parse_ipv4(ip_str);
    ip_send(dst_ip, IP_PROTO_TCP, (const uint8_t *)&conns[sockfd], sizeof(tcp_segment_t));
    
    spinlock_release(&tcp_lock);
    kprintf("tcp_connect: SYN to %s:%d\n", ip_str, port);
    return 0;
}

int tcp_send(int sockfd, const uint8_t* data, uint16_t len) {
    if (sockfd < 0 || sockfd >= MAX_TCP_CONNS || len > TCP_MAX_SEG) return -1;
    spinlock_acquire(&tcp_lock);
    
    conns[sockfd].hdr.flags = 0x10; // ACK + PSH
    memcpy(conns[sockfd].data, data, len);
    conns[sockfd].data_len = len;
    
    ip_send(0, IP_PROTO_TCP, (const uint8_t *)&conns[sockfd], sizeof(tcp_header_t) + len);
    
    spinlock_release(&tcp_lock);
    return len;
}

int tcp_recv(int sockfd, uint8_t* buf, uint16_t len) {
    // Simplified: blocking wait for data (use IPC for real async)
    kprintf("tcp_recv: waiting (sim)\n");
    (void)sockfd;
    (void)buf;
    (void)len;
    return 0;
}

int tcp_close(int sockfd) {
    kprintf("tcp_close: fd=%d\n", sockfd);
    memset(&conns[sockfd], 0, sizeof(tcp_segment_t));
    return 0;
}
