/*
 * Layer: 2 (Kernel Core) - Kernel Heap Implementation
 * File: heap.c
 * Purpose: Simple bitmap-based kernel heap allocator
 * Status: Simplified for Layer 2
 */

#include <heap.h>
#include <memory.h>
#include <kprintf.h>
#include <panic.h>
#include <string.h>

#define HEAP_ALLOC_MAGIC 0xC0DEC0DEu

typedef struct {
    uint32_t magic;
    uint32_t blocks;
    uint64_t requested_size;
} heap_alloc_header_t;

/* Global kernel heap */
heap_t kernel_heap;

/*
 * Initialize kernel heap
 */
void heap_init(uint64_t start_addr, uint64_t size)
{
    kprintf("Initializing kernel heap...\n");
    kprintf("  Start: 0x%llx, Size: %llu MB\n", start_addr, size / (1024 * 1024));

    /* Initialize heap structure */
    kernel_heap.heap_start = start_addr;
    kernel_heap.heap_size = size;
    kernel_heap.total_blocks = size / HEAP_BLOCK_SIZE;
    kernel_heap.used_blocks = 0;
    kernel_heap.peak_used_blocks = 0;
    kernel_heap.alloc_count = 0;
    kernel_heap.free_count = 0;
    kernel_heap.failed_allocations = 0;
    kernel_heap.invalid_frees = 0;

    /* Allocate bitmap */
    kernel_heap.bitmap = (uint8_t *)pmm_alloc_page();
    if (!kernel_heap.bitmap) {
        kprintf("Failed to allocate heap bitmap\n");
        return;
    }

    /* Clear bitmap (all blocks free) */
    memset(kernel_heap.bitmap, 0, HEAP_BITMAP_SIZE);

    /* Initialize spinlock */
    spinlock_init(&kernel_heap.lock);

    kprintf("Kernel heap initialized (%llu blocks)\n", kernel_heap.total_blocks);
}

/*
 * Allocate memory from kernel heap
 */
void *kmalloc(size_t size)
{
    if (size == 0) return NULL;

    size_t total_size = size + sizeof(heap_alloc_header_t);
    if (total_size < size) {
        kernel_heap.failed_allocations++;
        return NULL;
    }

    /* Calculate number of blocks needed */
    size_t blocks_needed = (total_size + HEAP_BLOCK_SIZE - 1) / HEAP_BLOCK_SIZE;
    size_t consecutive_free = 0;
    size_t start_block = 0;

    spinlock_lock(&kernel_heap.lock);

    /* Find consecutive free blocks */
    for (size_t i = 0; i < kernel_heap.total_blocks; i++) {
        size_t byte_index = i / 8;
        size_t bit_index = i % 8;

        if ((kernel_heap.bitmap[byte_index] & (1 << bit_index)) == 0) {
            if (consecutive_free == 0) {
                start_block = i;
            }
            consecutive_free++;

            if (consecutive_free == blocks_needed) {
                /* Found enough consecutive blocks */
                for (size_t j = 0; j < blocks_needed; j++) {
                    size_t block = start_block + j;
                    size_t b_byte = block / 8;
                    size_t b_bit = block % 8;
                    kernel_heap.bitmap[b_byte] |= (1 << b_bit);
                }

                kernel_heap.used_blocks += blocks_needed;
                if (kernel_heap.used_blocks > kernel_heap.peak_used_blocks) {
                    kernel_heap.peak_used_blocks = kernel_heap.used_blocks;
                }
                kernel_heap.alloc_count++;
                spinlock_unlock(&kernel_heap.lock);

                heap_alloc_header_t *header =
                    (heap_alloc_header_t *)(kernel_heap.heap_start + start_block * HEAP_BLOCK_SIZE);
                header->magic = HEAP_ALLOC_MAGIC;
                header->blocks = (uint32_t)blocks_needed;
                header->requested_size = size;
                return (void *)(header + 1);
            }
        } else {
            consecutive_free = 0;
        }
    }

    spinlock_unlock(&kernel_heap.lock);
    kernel_heap.failed_allocations++;
    return NULL; /* No suitable block found */
}

/*
 * Free memory from kernel heap
 */
void kfree(void *ptr)
{
    if (!ptr) return;

    heap_alloc_header_t *header = ((heap_alloc_header_t *)ptr) - 1;
    uint64_t addr = (uint64_t)header;
    if (addr < kernel_heap.heap_start ||
        addr >= kernel_heap.heap_start + kernel_heap.heap_size) {
        kernel_heap.invalid_frees++;
        return; /* Invalid pointer */
    }

    spinlock_lock(&kernel_heap.lock);

    if (header->magic != HEAP_ALLOC_MAGIC || header->blocks == 0) {
        kernel_heap.invalid_frees++;
        spinlock_unlock(&kernel_heap.lock);
        return;
    }

    /* Calculate block index */
    size_t start_block = (addr - kernel_heap.heap_start) / HEAP_BLOCK_SIZE;
    size_t blocks_to_free = header->blocks;

    /* Free the blocks */
    for (size_t i = 0; i < blocks_to_free; i++) {
        size_t block = start_block + i;
        if (block >= kernel_heap.total_blocks) break;

        size_t byte_index = block / 8;
        size_t bit_index = block % 8;

        if (kernel_heap.bitmap[byte_index] & (1 << bit_index)) {
            kernel_heap.bitmap[byte_index] &= ~(1 << bit_index);
            KASSERT(kernel_heap.used_blocks > 0, "heap used_blocks underflow");
            kernel_heap.used_blocks--;
        }
    }

    header->magic = 0;
    header->blocks = 0;
    header->requested_size = 0;
    kernel_heap.free_count++;
    spinlock_unlock(&kernel_heap.lock);
}

/*
 * Get size of allocated block (simplified)
 */
size_t ksize(void *ptr)
{
    if (!ptr) {
        return 0;
    }

    heap_alloc_header_t *header = ((heap_alloc_header_t *)ptr) - 1;
    if (header->magic != HEAP_ALLOC_MAGIC) {
        return 0;
    }

    return (size_t)header->requested_size;
}

/*
 * Debug print heap status
 */
void heap_debug_print(void)
{
    spinlock_lock(&kernel_heap.lock);
    kprintf("Heap Status: %llu/%llu blocks used\n",
            kernel_heap.used_blocks, kernel_heap.total_blocks);
    spinlock_unlock(&kernel_heap.lock);
}

/*
 * Print heap statistics
 */
void heap_print_stats(void)
{
    heap_debug_print();
}

void heap_get_stats(heap_stats_t *stats)
{
    if (!stats) {
        return;
    }

    spinlock_lock(&kernel_heap.lock);
    stats->total_blocks = kernel_heap.total_blocks;
    stats->used_blocks = kernel_heap.used_blocks;
    stats->peak_used_blocks = kernel_heap.peak_used_blocks;
    stats->alloc_count = kernel_heap.alloc_count;
    stats->free_count = kernel_heap.free_count;
    stats->failed_allocations = kernel_heap.failed_allocations;
    stats->invalid_frees = kernel_heap.invalid_frees;
    spinlock_unlock(&kernel_heap.lock);
}

/*
 * Test heap allocator
 */
void test_heap_allocator(void)
{
    kprintf("Testing kernel heap...\n");

    /* Allocate 50 blocks */
    void *blocks[50];
    for (int i = 0; i < 50; i++) {
        blocks[i] = kmalloc(64);
        if (!blocks[i]) {
            kprintf("  Alloc %d failed\n", i);
            return;
        }
    }
    kprintf("  50 blocks allocated\n");

    /* Free all blocks */
    for (int i = 0; i < 50; i++) {
        kfree(blocks[i]);
    }
    kprintf("  50 blocks freed\n");

    /* Check for leaks */
    if (kernel_heap.used_blocks == 0) {
        kprintf("  Heap test PASSED\n");
    } else {
        kprintf("  Heap test FAILED - %llu blocks leaked\n", kernel_heap.used_blocks);
    }
}
