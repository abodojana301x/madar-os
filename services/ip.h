/*
 * Layer: 4 (System Services) - IPv4 Protocol
 * File: ip.h
 * Purpose: IP header parsing, checksum, simple routing
 */

#ifndef IP_H
#define IP_H

#include <types.h>
#include "net_stack.h"

#define IP_MAX_ROUTES       8

typedef struct {
    uint32_t network;
    uint32_t netmask;
    uint32_t gateway;
    uint32_t iface_ip;
    int      valid;
} ip_route_t;

void ip_init(void);
int ip_receive(const uint8_t *packet, uint16_t len);
int ip_send(uint32_t dst_ip, uint8_t proto, const uint8_t *data, uint16_t len);
int ip_add_route(uint32_t network, uint32_t netmask, uint32_t gateway, uint32_t iface);
uint32_t ip_route_lookup(uint32_t dst);

#endif

