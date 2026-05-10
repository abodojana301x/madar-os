/*
 * Layer: 4 (System Services) - ARP Protocol
 * File: arp.h
 * Purpose: ARP cache, requests, and replies
 */

#ifndef ARP_H
#define ARP_H

#include <types.h>

#define ARP_CACHE_SIZE      16
#define ARP_HW_TYPE_ETH     1
#define ARP_PROTO_IP        0x0800
#define ARP_OP_REQUEST      1
#define ARP_OP_REPLY        2

typedef struct __attribute__((packed)) {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_len;
    uint8_t  proto_len;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint32_t sender_ip;
    uint8_t  target_mac[6];
    uint32_t target_ip;
} arp_packet_t;

typedef struct {
    uint32_t ip;
    uint8_t  mac[6];
    uint32_t timestamp;
    int      valid;
} arp_entry_t;

void arp_init(void);
int arp_resolve(uint32_t ip, uint8_t *mac_out);
void arp_receive(const uint8_t *packet, uint16_t len);
void arp_send_request(uint32_t target_ip);

#endif

