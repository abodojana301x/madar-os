/*
 * Layer: 4 (System Services) - Network Utilities
 * File: net_utils.c
 * Purpose: Checksum, byte order, and packet builders
 */

#include "net_stack.h"
#include <string.h>

uint16_t net_htons(uint16_t host) {
    return ((host & 0xFF) << 8) | ((host >> 8) & 0xFF);
}

uint16_t net_ntohs(uint16_t net) {
    return net_htons(net);
}

uint32_t net_htonl(uint32_t host) {
    return ((host & 0xFF) << 24) |
           ((host & 0xFF00) << 8) |
           ((host & 0xFF0000) >> 8) |
           ((host >> 24) & 0xFF);
}

uint32_t net_ntohl(uint32_t net) {
    return net_htonl(net);
}

uint16_t net_checksum(const void *data, uint16_t len) {
    const uint8_t *buf = (const uint8_t *)data;
    uint32_t sum = 0;

    while (len > 1) {
        sum += (buf[0] << 8) | buf[1];
        buf += 2;
        len -= 2;
    }

    if (len == 1) {
        sum += buf[0] << 8;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

int net_build_eth(eth_header_t *eth, const uint8_t *dst, const uint8_t *src, uint16_t type) {
    if (!eth || !dst || !src) return -1;
    memcpy(eth->dst_mac, dst, ETH_ALEN);
    memcpy(eth->src_mac, src, ETH_ALEN);
    eth->type = net_htons(type);
    return 0;
}

int net_build_ip(ip_header_t *ip, uint32_t src, uint32_t dst, uint8_t proto, uint16_t data_len) {
    if (!ip) return -1;
    ip->ver_ihl = 0x45; /* IPv4, IHL=5 (20 bytes) */
    ip->tos = 0;
    ip->total_len = net_htons(20 + data_len);
    ip->id = 0;
    ip->flags_frag = 0;
    ip->ttl = 64;
    ip->protocol = proto;
    ip->checksum = 0;
    ip->src_ip = net_htonl(src);
    ip->dst_ip = net_htonl(dst);
    ip->checksum = net_checksum(ip, 20);
    return 0;
}

int net_build_icmp_echo(icmp_echo_t *icmp, uint16_t id, uint16_t seq,
                        const uint8_t *payload, uint16_t payload_len) {
    if (!icmp) return -1;
    icmp->type = ICMP_ECHO_REQUEST;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->id = net_htons(id);
    icmp->sequence = net_htons(seq);

    uint16_t total_len = 8 + payload_len;
    uint8_t temp[NET_MAX_PACKET];
    memcpy(temp, icmp, 8);
    if (payload && payload_len > 0) {
        memcpy(temp + 8, payload, payload_len);
    }

    icmp->checksum = net_checksum(temp, total_len);
    return 0;
}

