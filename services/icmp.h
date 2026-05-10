/*
 * Layer: 4 (System Services) - ICMP Protocol
 * File: icmp.h
 * Purpose: Echo request/reply (ping) handling
 */

#ifndef ICMP_H
#define ICMP_H

#include <types.h>

void icmp_init(void);
int icmp_receive(const uint8_t *data, uint16_t len, uint32_t src_ip);
int icmp_send_echo(uint32_t dst_ip, uint16_t id, uint16_t seq);

#endif

