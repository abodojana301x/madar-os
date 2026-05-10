/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: VGA text mode implementation
 * Status: [ ] Testing [ ] Pass [ ] Failed
 */

#include <types.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_BUFFER;
static uint8_t vga_color = (0 << 4) | 7;
static uint16_t cursor_pos = 0;

void vga_clear(void);

void vga_init(void)
{
    vga_clear();
}

void vga_putc(char c)
{
    if (c == '\n') {
        cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
    } else {
        vga_buffer[cursor_pos++] = (uint16_t)c | ((uint16_t)vga_color << 8);
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        // Scroll up
        for (uint16_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        // Clear last line
        for (uint16_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
            vga_buffer[i] = (uint16_t)' ' | ((uint16_t)vga_color << 8);
        }
        cursor_pos = VGA_WIDTH * (VGA_HEIGHT - 1);
    }
}

void vga_puts(const char *str)
{
    while (*str) {
        vga_putc(*str++);
    }
}

void vga_clear(void)
{
    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (uint16_t)' ' | ((uint16_t)vga_color << 8);
    }
    cursor_pos = 0;
}

void vga_set_color(uint8_t fg, uint8_t bg)
{
    vga_color = (bg << 4) | fg;
}