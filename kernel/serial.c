/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Serial port implementation
 * Status: [ ] Testing [ ] Pass [ ] Failed
 */

#include <types.h>

#define SERIAL_COM1 0x3F8
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void serial_init(void)
{
    uint16_t base = SERIAL_COM1;

    // Disable interrupts
    outb(SERIAL_LINE_COMMAND_PORT(base), 0x00);

    // Enable DLAB
    outb(SERIAL_LINE_COMMAND_PORT(base), 0x80);

    // Set baud rate to 115200
    outb(SERIAL_DATA_PORT(base), 0x01);
    outb(SERIAL_DATA_PORT(base) + 1, 0x00);

    // 8 bits, no parity, one stop bit
    outb(SERIAL_LINE_COMMAND_PORT(base), 0x03);

    // Enable FIFO
    outb(SERIAL_FIFO_COMMAND_PORT(base), 0xC7);

    // Enable IRQs, RTS/DSR
    outb(SERIAL_MODEM_COMMAND_PORT(base), 0x0B);
}

void serial_putc(char c)
{
    uint16_t base = SERIAL_COM1;

    while ((inb(SERIAL_LINE_STATUS_PORT(base)) & 0x20) == 0);
    outb(SERIAL_DATA_PORT(base), c);
}

void serial_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            serial_putc('\r');
        }
        serial_putc(*str++);
    }
}

int serial_received(void)
{
    uint16_t base = SERIAL_COM1;
    return inb(SERIAL_LINE_STATUS_PORT(base)) & 1;
}

char serial_read(void)
{
    uint16_t base = SERIAL_COM1;
    while (!serial_received());
    return inb(SERIAL_DATA_PORT(base));
}