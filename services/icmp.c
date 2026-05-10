/*
 * Layer: 4 (System Services) - ICMP Implementation
 * File: icmp.c
 * Purpose: Echo request/reply (ping) handling
 */

#include "icmp.h"
#include "net_stack.h"
#include "ip.h"
#include <kprintf.h>
#include <string.h>

void icmp_init(void) {
    kprintf("🌐 Initializing ICMP...\n");
    kprintf("✅ ICMP initialized\n");
}

int icmp_receive(const uint8_t *data, uint16_t len, uint32_t src_ip) {
    if (!data || len < sizeof(icmp_echo_t)) return -1;

    icmp_echo_t *icmp = (icmp_echo_t *)data;
    uint8_t type = icmp->type;
    uint8_t code = icmp->code;
    (void)code;

    if (type == ICMP_ECHO_REQUEST) {
        uint16_t id = net_ntohs(icmp->id);
        uint16_t seq = net_ntohs(icmp->sequence);

        kprintf("  ICMP Echo Request from %u.%u.%u.%u id=%u seq=%u\n",
                (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF,
                (src_ip >> 8) & 0xFF, src_ip & 0xFF,
                id, seq);

        /* Send reply */
        return icmp_send_echo(src_ip, id, seq);
    } else if (type == ICMP_ECHO_REPLY) {
        uint16_t id = net_ntohs(icmp->id);
        uint16_t seq = net_ntohs(icmp->sequence);
        kprintf("  ICMP Echo Reply from %u.%u.%u.%u id=%u seq=%u\n",
                (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF,
                (src_ip >> 8) & 0xFF, src_ip & 0xFF,
                id, seq);
        return 0;
    }

    return -1;
}

int icmp_send_echo(uint32_t dst_ip, uint16_t id, uint16_t seq) {
    uint8_t packet[NET_MAX_PACKET];
    icmp_echo_t *icmp = (icmp_echo_t *)packet;
    uint8_t *payload = packet + sizeof(icmp_echo_t);

    /* Simple payload */
    const char *ping_data = "MadarOS";
    uint16_t payload_len = 7;
    memcpy(payload, ping_data, payload_len);

    net_build_icmp_echo(icmp, id, seq, payload, payload_len);

    kprintf("  ICMP Echo Reply sent to %u.%u.%u.%u\n",
            (dst_ip >> 24) & 0xFF, (dst_ip >> 16) & 0xFF,
            (dst_ip >> 8) & 0xFF, dst_ip & 0xFF);

    return ip_send(dst_ip, IP_PROTO_ICMP, packet, sizeof(icmp_echo_t) + payload_len);
}

