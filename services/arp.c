/*
 * Layer: 4 (System Services) - ARP Implementation
 * File: arp.c
 * Purpose: ARP cache, requests, and replies
 */

#include "arp.h"
#include "net_stack.h"
#include <kprintf.h>
#include <string.h>

static arp_entry_t arp_cache[ARP_CACHE_SIZE];
static uint8_t local_mac[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static uint32_t local_ip = 0;

void arp_init(void) {
    kprintf("🌐 Initializing ARP...\n");
    memset(arp_cache, 0, sizeof(arp_cache));
    kprintf("✅ ARP initialized\n");
}

void arp_set_local(uint32_t ip, const uint8_t *mac) {
    local_ip = ip;
    if (mac) memcpy(local_mac, mac, 6);
}

static int arp_find_entry(uint32_t ip) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].valid && arp_cache[i].ip == ip) {
            return i;
        }
    }
    return -1;
}

static int arp_add_entry(uint32_t ip, const uint8_t *mac) {
    int idx = arp_find_entry(ip);
    if (idx < 0) {
        /* Find free slot */
        for (int i = 0; i < ARP_CACHE_SIZE; i++) {
            if (!arp_cache[i].valid) {
                idx = i;
                break;
            }
        }
    }
    if (idx < 0) {
        /* Evict oldest */
        idx = 0;
    }

    arp_cache[idx].ip = ip;
    memcpy(arp_cache[idx].mac, mac, 6);
    arp_cache[idx].valid = 1;
    return 0;
}

int arp_resolve(uint32_t ip, uint8_t *mac_out) {
    if (!mac_out) return -1;

    /* Check cache */
    int idx = arp_find_entry(ip);
    if (idx >= 0) {
        memcpy(mac_out, arp_cache[idx].mac, 6);
        return 0;
    }

    /* Not in cache - would send ARP request here */
    kprintf("  ARP: %u.%u.%u.%u not in cache, requesting...\n",
            (ip >> 24) & 0xFF, (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF, ip & 0xFF);

    return -1; /* Not resolved yet */
}

void arp_receive(const uint8_t *packet, uint16_t len) {
    if (!packet || len < sizeof(arp_packet_t)) return;

    arp_packet_t *arp = (arp_packet_t *)packet;
    uint16_t opcode = net_ntohs(arp->opcode);

    if (opcode == ARP_OP_REPLY) {
        uint32_t sender_ip = net_ntohl(arp->sender_ip);
        kprintf("  ARP Reply: %u.%u.%u.%u -> %02X:%02X:%02X:%02X:%02X:%02X\n",
                (sender_ip >> 24) & 0xFF, (sender_ip >> 16) & 0xFF,
                (sender_ip >> 8) & 0xFF, sender_ip & 0xFF,
                arp->sender_mac[0], arp->sender_mac[1], arp->sender_mac[2],
                arp->sender_mac[3], arp->sender_mac[4], arp->sender_mac[5]);
        arp_add_entry(sender_ip, arp->sender_mac);
    } else if (opcode == ARP_OP_REQUEST) {
        uint32_t target_ip = net_ntohl(arp->target_ip);
        if (target_ip == local_ip) {
            kprintf("  ARP Request for us - would reply\n");
        }
    }
}

void arp_send_request(uint32_t target_ip) {
    (void)target_ip;
    kprintf("  Sending ARP request for %u.%u.%u.%u\n",
            (target_ip >> 24) & 0xFF, (target_ip >> 16) & 0xFF,
            (target_ip >> 8) & 0xFF, target_ip & 0xFF);
}

