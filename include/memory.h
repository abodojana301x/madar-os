/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: إدارة الذاكرة الفيزيائية البسيطة
 * Constraints: لا يدعم virtual memory في هذه المرحلة
 * Dependencies: multiboot2.h
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>

#define PAGE_SIZE 4096
#define MAX_MEMORY_REGIONS 32

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;  // 1 = usable, others = reserved
} memory_region_t;

typedef struct {
    uint64_t total_memory;
    uint64_t usable_memory;
    uint64_t next_free_page;
    uint64_t total_pages;
    uint64_t free_pages;
} memory_map_t;

// Function prototypes
void memory_init(uint64_t multiboot_info_ptr);
uint64_t pmm_alloc_page(void);
void pmm_free_page(uint64_t addr);
void memory_print_info(void);

// Test function
void test_memory_allocator(void);

#endif