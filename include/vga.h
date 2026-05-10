/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: VGA text mode output
 * Constraints: 80x25 text mode only
 */

#ifndef VGA_H
#define VGA_H

#include <types.h>

// VGA constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER 0xB8000

// Colors
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

// VGA entry
#define VGA_ENTRY(color, c) ((uint16_t)c | (uint16_t)color << 8)

// Function prototypes
void vga_init(void);
void vga_putc(char c);
void vga_puts(const char *str);
void vga_clear(void);
void vga_set_color(uint8_t fg, uint8_t bg);

#endif