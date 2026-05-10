/*
 * Layer: 3 (Drivers & Hardware Abstraction) - RTL8139 Network
 * File: rtl8139.h
 * Purpose: RTL8139 Ethernet driver - Complete header
 * Status: Phase 5 - Full RX/TX ring support
 */

#ifndef RTL8139_H
#define RTL8139_H

#include <types.h>

/* RTL8139 PCI Vendor/Device IDs */
#define RTL8139_VENDOR_ID   0x10EC
#define RTL8139_DEVICE_ID   0x8139

/* RTL8139 Register Offsets */
#define RTL_REG_MAC0        0x00  /* MAC Address 0-1 */
#define RTL_REG_MAC4        0x04  /* MAC Address 4-5 */
#define RTL_REG_TXSTATUS0   0x10  /* Transmit Status 0 */
#define RTL_REG_TXSTATUS1   0x14  /* Transmit Status 1 */
#define RTL_REG_TXSTATUS2   0x18  /* Transmit Status 2 */
#define RTL_REG_TXSTATUS3   0x1C  /* Transmit Status 3 */
#define RTL_REG_TXADDR0     0x20  /* Transmit Start Address 0 */
#define RTL_REG_TXADDR1     0x24  /* Transmit Start Address 1 */
#define RTL_REG_TXADDR2     0x28  /* Transmit Start Address 2 */
#define RTL_REG_TXADDR3     0x2C  /* Transmit Start Address 3 */
#define RTL_REG_RXBUFADDR   0x30  /* RX Buffer Address */
#define RTL_REG_RXBUFPTR    0x38  /* RX Buffer Read Pointer */
#define RTL_REG_RXBUFTAIL   0x3A  /* RX Buffer Tail Pointer */
#define RTL_REG_RXEARLYCNT  0x34  /* RX Early Count */
#define RTL_REG_RXCONFIG    0x44  /* RX Config */
#define RTL_REG_TXCONFIG    0x40  /* TX Config */
#define RTL_REG_CMD         0x37  /* Command Register */
#define RTL_REG_IMASK       0x3C  /* Interrupt Mask */
#define RTL_REG_ISTAT       0x3E  /* Interrupt Status */
#define RTL_REG_MULTIINT    0x3C  /* Multiple Interrupt Select */

/* Shorthand aliases */
#define RTL_REG_TXSTATUS    RTL_REG_TXSTATUS0
#define RTL_REG_TXADDR      RTL_REG_TXADDR0

/* RTL8139 Commands */
#define RTL_CMD_RXENABLE    0x08
#define RTL_CMD_TXENABLE    0x04
#define RTL_CMD_RESET       0x10

/* RTL8139 Buffer Sizes */
#define RTL_RX_BUFFER_SIZE  0x2000  /* 8KB */
#define RTL_TX_BUFFER_SIZE  0x2000  /* 8KB */
#define RTL_MAX_PACKETS     4

typedef struct {
    uint32_t iobase;
    uint32_t mem_addr;
    uint8_t mac[6];
    uint8_t tx_buffer_index;
} rtl8139_t;

/* Statistics structure */
typedef struct {
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t tx_errors;
    uint32_t rx_errors;
    uint32_t tx_buffer_index;
} rtl8139_stats_t;

extern rtl8139_t rtl8139_dev;

/* Function Prototypes */
void rtl8139_init(uint32_t iobase, uint32_t mem_addr);
void rtl8139_send_packet(const uint8_t *data, uint16_t length);
uint16_t rtl8139_recv_packet(uint8_t *buffer, uint16_t max_length);
void rtl8139_get_mac(uint8_t *mac);
void rtl8139_get_stats(rtl8139_stats_t *stats);

#endif /* RTL8139_H */
