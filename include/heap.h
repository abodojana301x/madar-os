/*
 * Layer: 2 (Kernel Core) - Kernel Heap Manager
 * File: heap.h
 * Purpose: Kernel heap allocation interface
 * Status: New for Layer 2
 */

#ifndef HEAP_H
#define HEAP_H

#include <types.h>
#include <memory_layout.h>
#include <spinlock.h>

/* Heap Configuration */
#define HEAP_BLOCK_SIZE     16      /* 16-byte blocks */
#define HEAP_BITMAP_SIZE    8192    /* 16MB / 16 bytes / 8 bits = 8192 bytes */

/* Heap Structure */
typedef struct {
    uint8_t *bitmap;
    uint64_t total_blocks;
    uint64_t used_blocks;
    uint64_t peak_used_blocks;
    uint64_t heap_start;
    uint64_t heap_size;
    uint64_t alloc_count;
    uint64_t free_count;
    uint64_t failed_allocations;
    uint64_t invalid_frees;
    spinlock_t lock;
} heap_t;

typedef struct {
    uint64_t total_blocks;
    uint64_t used_blocks;
    uint64_t peak_used_blocks;
    uint64_t alloc_count;
    uint64_t free_count;
    uint64_t failed_allocations;
    uint64_t invalid_frees;
} heap_stats_t;

/* Global heap instance */
extern heap_t kernel_heap;

/* Function Prototypes */
void heap_init(uint64_t start_addr, uint64_t size);
void *kmalloc(size_t size);
void kfree(void *ptr);
size_t ksize(void *ptr);

/* Debug functions */
void heap_debug_print(void);
void heap_print_stats(void);
void heap_get_stats(heap_stats_t *stats);
void test_heap_allocator(void);

#endif /* HEAP_H */
