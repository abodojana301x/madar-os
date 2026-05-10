#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "../include/types.h"
#include "../include/kprintf.h"

/* Color Definitions (RGB 24-bit) */
#define COLOR_BLACK         0x000000
#define COLOR_WHITE         0xFFFFFF
#define COLOR_RED           0xFF0000
#define COLOR_GREEN         0x00FF00
#define COLOR_BLUE          0x0000FF
#define COLOR_CYAN          0x00FFFF
#define COLOR_MAGENTA       0xFF00FF
#define COLOR_YELLOW        0xFFFF00
#define COLOR_GRAY          0x808080
#define COLOR_LIGHT_GRAY    0xC0C0C0
#define COLOR_DARK_GRAY     0x404040

/* Standard Resolutions */
#define RES_640x480     0
#define RES_800x600     1
#define RES_1024x768    2
#define RES_1280x1024   3
#define RES_1920x1080   4

/* Framebuffer Structure */
typedef struct {
    uint32_t *base;       /* Base address of framebuffer in memory */
    uint32_t width;       /* Framebuffer width in pixels */
    uint32_t height;      /* Framebuffer height in pixels */
    uint32_t pitch;       /* Bytes per horizontal line */
    uint32_t bpp;         /* Bits per pixel */
    uint8_t  red_mask;    /* Red channel bits */
    uint8_t  green_mask;  /* Green channel bits */
    uint8_t  blue_mask;   /* Blue channel bits */
    uint8_t  red_pos;     /* Red channel bit position */
    uint8_t  green_pos;   /* Green channel bit position */
    uint8_t  blue_pos;    /* Blue channel bit position */
    uint32_t initialized;
    uint32_t resolution_id;
} framebuffer_t;

/* VBE Controller Info Block (from VGA BIOS) */
typedef struct __attribute__((packed)) {
    uint8_t  signature[4];     /* "VESA" */
    uint16_t version;
    uint32_t oem_string_ptr;
    uint32_t capabilities;
    uint32_t video_mode_ptr;
    uint16_t total_memory_64k;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name_ptr;
    uint32_t oem_product_name_ptr;
    uint32_t oem_product_rev_ptr;
    uint8_t  reserved[222];
    uint8_t  oem_data[256];
} vbe_controller_info_t;

/* VBE Mode Info Block */
typedef struct __attribute__((packed)) {
    uint16_t attributes;
    uint8_t  win_a_attrs;
    uint8_t  win_b_attrs;
    uint16_t win_granularity;
    uint16_t win_size;
    uint16_t win_a_seg;
    uint16_t win_b_seg;
    uint32_t win_func_ptr;
    uint16_t bytes_per_scanline;
    uint16_t width;
    uint16_t height;
    uint8_t  char_width;
    uint8_t  char_height;
    uint8_t  num_planes;
    uint8_t  bpp;
    uint8_t  num_banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  num_image_pages;
    uint8_t  reserved1;
    uint8_t  red_mask_size;
    uint8_t  red_field_pos;
    uint8_t  green_mask_size;
    uint8_t  green_field_pos;
    uint8_t  blue_mask_size;
    uint8_t  blue_field_pos;
    uint8_t  rsvd_mask_size;
    uint8_t  rsvd_field_pos;
    uint8_t  directcolor_info;
    uint32_t phys_base_ptr;
    uint32_t offscreen_ptr;
    uint16_t offscreen_size;
    uint8_t  reserved2[206];
} vbe_mode_info_t;

extern framebuffer_t fb_global;

/* Function Prototypes */
void fb_init(void);
int fb_init_from_multiboot(uint64_t multiboot_ptr);
int fb_try_resolution(uint32_t width, uint32_t height, uint32_t bpp);
int fb_set_resolution(uint32_t res_id);
void fb_query_vbe(void);
void fb_put_pixel(int x, int y, uint32_t color);
void fb_draw_rect(int x, int y, int width, int height, uint32_t color);
void fb_draw_filled_rect(int x, int y, int width, int height, uint32_t color);
void fb_clear(uint32_t color);
void fb_draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void fb_print_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color);

#endif /* _FRAMEBUFFER_H */
