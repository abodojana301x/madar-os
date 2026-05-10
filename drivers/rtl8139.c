/*
 * Layer: 3 (Drivers & Hardware Abstraction) - RTL8139 Network
 * File: rtl8139.c
 * Purpose: RTL8139 Ethernet driver - Full RX/TX ring implementation
 * Status: Phase 5 - Complete with ring buffers
 */

#include "rtl8139.h"
#include <kprintf.h>
#include <string.h>
#include <io.h>
#include "../drivers/irq_manager.h"

static rtl8139_t rtl8139_global = {0};
rtl8139_t rtl8139_dev = {0};

/* Internal TX ring */
static uint8_t tx_ring[RTL_TX_BUFFER_SIZE] __attribute__((aligned(256)));
static uint32_t tx_ring_head = 0;
static uint32_t tx_ring_tail = 0;
static uint32_t tx_ring_count = 0;

/* Internal RX ring buffer */
static uint8_t rx_ring[RTL_RX_BUFFER_SIZE + 16] __attribute__((aligned(16)));
static uint32_t rx_offset = 0;

/* Packet statistics */
static uint32_t packets_sent = 0;
static uint32_t packets_received = 0;
static uint32_t tx_errors = 0;
static uint32_t rx_errors = 0;

/* RTL8139 interrupt handler */
static void rtl8139_irq_handler(void)
{
    if (!rtl8139_global.iobase) return;

    uint16_t status = inw(rtl8139_global.iobase + RTL_REG_ISTAT);

    /* Acknowledge interrupts */
    outw(rtl8139_global.iobase + RTL_REG_ISTAT, status);

    if (status & 0x01) { /* ROK - Receive OK */
        packets_received++;
    }
    if (status & 0x04) { /* TOK - Transmit OK */
        packets_sent++;
    }
    if (status & 0x10) { /* TXE - Transmit Error */
        tx_errors++;
    }
    if (status & 0x20) { /* RXE - Receive Error */
        rx_errors++;
    }
}

/* Initialize RTL8139 network card */
void rtl8139_init(uint32_t iobase, uint32_t mem_addr)
{
    uint16_t i;

    kprintf("🌐 Initializing RTL8139 Network Card...\n");

    rtl8139_global.iobase = iobase;
    rtl8139_global.mem_addr = mem_addr;
    rtl8139_global.tx_buffer_index = 0;

    /* Soft reset */
    outb(iobase + RTL_REG_CMD, RTL_CMD_RESET);
    for (i = 0; i < 1000; i++) {
        if (!(inb(iobase + RTL_REG_CMD) & RTL_CMD_RESET)) break;
    }

    /* Read MAC address */
    for (i = 0; i < 6; i++) {
        rtl8139_global.mac[i] = inb(iobase + RTL_REG_MAC0 + i);
    }

    /* Power on */
    outb(iobase + 0x52, 0x00);

    /* Initialize RX buffer */
    rx_offset = 0;
    memset(rx_ring, 0, sizeof(rx_ring));
    outl(iobase + RTL_REG_RXBUFADDR, (uint32_t)(uintptr_t)rx_ring);

    /* Configure RX mode */
    outl(iobase + RTL_REG_RXCONFIG, 0x00000F00);

    /* Enable RX and TX */
    outb(iobase + RTL_REG_CMD, RTL_CMD_RXENABLE | RTL_CMD_TXENABLE);

    /* Configure TX: use TX config register */
    outl(iobase + RTL_REG_TXCONFIG, 0x03000000);

    /* Enable interrupts (ROK, TOK, TXE, RXE) */
    outw(iobase + RTL_REG_IMASK, 0x0035);

    /* Register IRQ handler */
    irq_register_handler(IRQ_NETWORK, rtl8139_irq_handler);

    kprintf("✅ RTL8139 initialized - MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            rtl8139_global.mac[0], rtl8139_global.mac[1],
            rtl8139_global.mac[2], rtl8139_global.mac[3],
            rtl8139_global.mac[4], rtl8139_global.mac[5]);

    /* Copy to export */
    rtl8139_dev = rtl8139_global;
}

/* Send an Ethernet packet */
void rtl8139_send_packet(const uint8_t *data, uint16_t length)
{
    uint32_t tx_addr;
    uint32_t status;

    if (!data || length == 0 || !rtl8139_global.iobase) {
        return;
    }

    if (length > RTL_TX_BUFFER_SIZE) {
        length = RTL_TX_BUFFER_SIZE;
    }

    kprintf("📤 RTL8139: Sending %u byte packet\n", length);

    /* Copy packet to TX ring buffer */
    memcpy(tx_ring, data, length);

    /* Set TX address (TSAD registers 0-3) */
    tx_addr = (uint32_t)(uintptr_t)tx_ring;
    outl(rtl8139_global.iobase + RTL_REG_TXADDR + (rtl8139_global.tx_buffer_index * 4), tx_addr);

    /* Set TX status to start transmission */
    status = length & 0x00001FFF;
    status |= 0x00100000; /* Start transmission */
    outl(rtl8139_global.iobase + RTL_REG_TXSTATUS + (rtl8139_global.tx_buffer_index * 4), status);

    /* Advance TX buffer index */
    rtl8139_global.tx_buffer_index = (rtl8139_global.tx_buffer_index + 1) % RTL_MAX_PACKETS;

    packets_sent++;
}

/* Receive an Ethernet packet */
uint16_t rtl8139_recv_packet(uint8_t *buffer, uint16_t max_length)
{
    uint16_t length;
    uint16_t rx_status;

    if (!buffer || !rtl8139_global.iobase || max_length == 0) {
        return 0;
    }

    /* Read RX status and length from first 4 bytes of next packet */
    rx_status = *(volatile uint16_t *)(rx_ring + rx_offset);
    length = *(volatile uint16_t *)(rx_ring + rx_offset + 2);

    /* Bit 0 of status indicates packet is ready */
    if (!(rx_status & 0x0001)) {
        /* No packet available */
        /* Reset RX if we wrapped around */
        if (rx_offset > 0) {
            rx_offset = 0;
        }
        return 0;
    }

    /* Packet length (bits 0-13), minus 4-byte CRC */
    length = (length & 0x3FFF) - 4;

    /* Safety check */
    if (length > max_length || length > RTL_RX_BUFFER_SIZE) {
        rx_errors++;
        rx_offset = 0;
        outw(rtl8139_global.iobase + RTL_REG_RXBUFPTR, rx_offset);
        return 0;
    }

    /* Copy packet data (skip 4-byte header) */
    memcpy(buffer, rx_ring + rx_offset + 4, length);

    /* Update RX offset and wrap if needed */
    rx_offset = (rx_offset + length + 4 + 3) & ~3;
    if (rx_offset >= RTL_RX_BUFFER_SIZE) {
        rx_offset = 0;
    }

    /* Tell the card we've read the data */
    outw(rtl8139_global.iobase + RTL_REG_RXBUFPTR, rx_offset);

    packets_received++;
    return length;
}

/* Get MAC address */
void rtl8139_get_mac(uint8_t *mac)
{
    if (mac) {
        for (int i = 0; i < 6; i++) {
            mac[i] = rtl8139_global.mac[i];
        }
    }
}

/* Get statistics */
void rtl8139_get_stats(rtl8139_stats_t *stats)
{
    if (stats) {
        stats->packets_sent = packets_sent;
        stats->packets_received = packets_received;
        stats->tx_errors = tx_errors;
        stats->rx_errors = rx_errors;
        stats->tx_buffer_index = rtl8139_global.tx_buffer_index;
    }
}
