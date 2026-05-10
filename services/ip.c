/*
 * Layer: 4 (System Services) - IPv4 Implementation
 * File: ip.c
 * Purpose: IP header parsing, checksum, simple routing
 */

#include "ip.h"
#include <kprintf.h>
#include <string.h>

static ip_route_t routes[IP_MAX_ROUTES];
static uint32_t local_ip = 0;

void ip_init(void) {
    kprintf("🌐 Initializing IPv4...\n");
    memset(routes, 0, sizeof(routes));
    kprintf("✅ IPv4 initialized\n");
}

void ip_set_local(uint32_t ip) {
    local_ip = ip;
}

int ip_add_route(uint32_t network, uint32_t netmask, uint32_t gateway, uint32_t iface) {
    for (int i = 0; i < IP_MAX_ROUTES; i++) {
        if (!routes[i].valid) {
            routes[i].network = network;
            routes[i].netmask = netmask;
            routes[i].gateway = gateway;
            routes[i].iface_ip = iface;
            routes[i].valid = 1;
            kprintf("  Route added: %u.%u.%u.%u/%u gw %u.%u.%u.%u\n",
                    (network >> 24) & 0xFF, (network >> 16) & 0xFF,
                    (network >> 8) & 0xFF, network & 0xFF,
                    netmask, (gateway >> 24) & 0xFF, (gateway >> 16) & 0xFF,
                    (gateway >> 8) & 0xFF, gateway & 0xFF);
            return 0;
        }
    }
    return -1;
}

uint32_t ip_route_lookup(uint32_t dst) {
    for (int i = 0; i < IP_MAX_ROUTES; i++) {
        if (routes[i].valid && (dst & routes[i].netmask) == routes[i].network) {
            return routes[i].gateway;
        }
    }
    /* Default: assume directly reachable */
    return dst;
}

int ip_receive(const uint8_t *packet, uint16_t len) {
    if (!packet || len < sizeof(ip_header_t)) return -1;

    ip_header_t *ip = (ip_header_t *)packet;
    uint8_t version = (ip->ver_ihl >> 4) & 0x0F;
    uint8_t ihl = ip->ver_ihl & 0x0F;

    if (version != 4) {
        kprintf("  IP: not IPv4 (ver=%u)\n", version);
        return -1;
    }

    uint16_t hdr_checksum = ip->checksum;
    ip->checksum = 0;
    if (net_checksum(ip, ihl * 4) != hdr_checksum) {
        kprintf("  IP: checksum mismatch\n");
        return -1;
    }

    uint32_t src = net_ntohl(ip->src_ip);
    uint32_t dst = net_ntohl(ip->dst_ip);
    uint8_t proto = ip->protocol;
    uint16_t total_len = net_ntohs(ip->total_len);
    (void)src;
    (void)total_len;

    if (dst != local_ip && dst != 0xFFFFFFFF) {
        /* Not for us */
        return -1;
    }

    const uint8_t *data = packet + (ihl * 4);
    uint16_t data_len = len - (ihl * 4);

    if (proto == IP_PROTO_ICMP) {
        extern int icmp_receive(const uint8_t *data, uint16_t len, uint32_t src_ip);
        return icmp_receive(data, data_len, src);
    }

    return 0;
}

int ip_send(uint32_t dst_ip, uint8_t proto, const uint8_t *data, uint16_t len) {
    if (!data || len == 0) return -1;

    uint8_t packet[NET_MAX_PACKET];
    if (len + sizeof(ip_header_t) + sizeof(eth_header_t) > NET_MAX_PACKET) {
        return -1;
    }

    eth_header_t *eth = (eth_header_t *)packet;
    ip_header_t *ip = (ip_header_t *)(packet + sizeof(eth_header_t));
    uint8_t *payload = packet + sizeof(eth_header_t) + sizeof(ip_header_t);

    /* Build IP header */
    net_build_ip(ip, local_ip, dst_ip, proto, len);

    /* Copy payload */
    memcpy(payload, data, len);

    /* Resolve MAC */
    uint8_t dst_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t src_mac[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};

    /* Build Ethernet header */
    net_build_eth(eth, dst_mac, src_mac, ETHERTYPE_IP);

    uint16_t total_len = sizeof(eth_header_t) + sizeof(ip_header_t) + len;
    (void)total_len;

    kprintf("  IP: sending %u bytes to %u.%u.%u.%u\n", len,
            (dst_ip >> 24) & 0xFF, (dst_ip >> 16) & 0xFF,
            (dst_ip >> 8) & 0xFF, dst_ip & 0xFF);

    return 0;
}

