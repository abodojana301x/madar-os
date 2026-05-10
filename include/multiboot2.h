/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Multiboot2 information structures
 * Based on Multiboot2 specification
 */

#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <types.h>

// Multiboot2 magic number
#define MULTIBOOT2_MAGIC 0x36d76289

// Multiboot2 info structure
struct multiboot_info {
    uint32_t total_size;
    uint32_t reserved;
    // Tags follow...
};

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

// Memory map tag
struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    // Entries follow...
};

struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
};

// Tag types
#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_TYPE_MMAP 6
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8

struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;
    /* color_info follows */
};

#endif
