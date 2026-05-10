/*
 * Layer: 4 (System Services) - Network Stack
 * File: net_stack.h
 * Purpose: Ethernet, IP, and ICMP shared structures
 */

#ifndef NET_STACK_H
#define NET_STACK_H

#include <types.h>

/* Ethernet */
#define ETH_ALEN        6
#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

typedef struct __attribute__((packed)) {
    uint8_t  dst_mac[ETH_ALEN];
    uint8_t  src_mac[ETH_ALEN];
    uint16_t type;
} eth_header_t;

/* IPv4 */
typedef struct __attribute__((packed)) {
    uint8_t  ver_ihl;       /* Version (4) | IHL (5) */
    uint8_t  tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} ip_header_t;

#define IP_PROTO_ICMP   1
#define IP_PROTO_TCP    6
#define IP_PROTO_UDP    17

/* ICMP */
typedef struct __attribute__((packed)) {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
} icmp_echo_t;

#define ICMP_ECHO_REPLY     0
#define ICMP_ECHO_REQUEST   8

/* Packet buffer */
#define NET_MAX_PACKET      2048

typedef struct {
    uint8_t data[NET_MAX_PACKET];
    uint16_t len;
} net_packet_t;

/* Utility functions */
uint16_t net_checksum(const void *data, uint16_t len);
uint16_t net_htons(uint16_t host);
uint16_t net_ntohs(uint16_t net);
uint32_t net_htonl(uint32_t host);
uint32_t net_ntohl(uint32_t net);

/* Packet building */
int net_build_eth(eth_header_t *eth, const uint8_t *dst, const uint8_t *src, uint16_t type);
int net_build_ip(ip_header_t *ip, uint32_t src, uint32_t dst, uint8_t proto, uint16_t data_len);
int net_build_icmp_echo(icmp_echo_t *icmp, uint16_t id, uint16_t seq, const uint8_t *payload, uint16_t payload_len);

/* IP address helpers */
static inline uint32_t ip_from_bytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | (uint32_t)d;
}

static inline void ip_to_str(uint32_t ip, char *buf) {
    uint8_t *b = (uint8_t *)&ip;
    /* Simple itoa-like formatting would go here */
    (void)b;
    buf[0] = '?';
    buf[1] = '\0';
}

#endif

