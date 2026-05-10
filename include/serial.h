/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Serial port output for debugging
 * Constraints: COM1 only, 115200 baud
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <types.h>

// Serial ports
#define SERIAL_COM1 0x3F8

// Function prototypes
void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *str);
int serial_received(void);
char serial_read(void);

#endif