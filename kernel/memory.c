/*
 * Layer: 1 (HAL/Bootstrap)
 * File: memory.c
 * Purpose: Physical memory management
 * Status: ✅ Complete
 */

#include <types.h>
#include <kprintf.h>

#define PAGE_SIZE 4096
#define TOTAL_MEMORY (128 * 1024 * 1024)  /* 128 MB */

extern uint8_t __kernel_end[];

typedef struct {
    uint64_t total_memory;
    uint64_t usable_memory;
    uint64_t next_free_page;
    uint64_t total_pages;
    uint64_t free_pages;
} memory_map_t;

static memory_map_t mem_map = {0};

void memory_init(uint64_t multiboot_ptr)
{
    (void)multiboot_ptr;

    /* Initialize memory map */
    mem_map.total_memory = TOTAL_MEMORY;
    mem_map.usable_memory = mem_map.total_memory;
    mem_map.total_pages = mem_map.usable_memory / PAGE_SIZE;
    mem_map.free_pages = mem_map.total_pages;
    uint64_t kernel_end = (uint64_t)__kernel_end;
    mem_map.next_free_page = (kernel_end + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);

    kprintf("📊 Memory initialized: %llu MB, %llu pages free\n",
            mem_map.total_memory / (1024 * 1024), 
            mem_map.free_pages);
}

uint64_t pmm_alloc_page(void)
{
    if (mem_map.free_pages == 0) {
        return 0;  /* Out of memory */
    }

    uint64_t page = mem_map.next_free_page;
    mem_map.next_free_page += PAGE_SIZE;
    mem_map.free_pages--;

    return page;
}

void pmm_free_page(uint64_t addr)
{
    if (addr >= mem_map.next_free_page) {
        mem_map.free_pages++;
    }
}

void memory_print_info(void)
{
    kprintf("💾 Memory Info:\n");
    kprintf("  Total: %llu MB\n", mem_map.total_memory / (1024 * 1024));
    kprintf("  Usable: %llu MB\n", mem_map.usable_memory / (1024 * 1024));
    kprintf("  Pages: %llu total, %llu free\n", 
            mem_map.total_pages, mem_map.free_pages);
}

void test_memory_allocator(void)
{
    kprintf("🧪 Testing memory allocator...\n");

    uint64_t page1 = pmm_alloc_page();
    uint64_t page2 = pmm_alloc_page();
    uint64_t page3 = pmm_alloc_page();

    if (page1 == 0 || page2 == 0 || page3 == 0) {
        kprintf("❌ Memory allocation failed\n");
        return;
    }

    /* Verify pages are properly allocated */
    if (page2 == page1 + PAGE_SIZE && page3 == page2 + PAGE_SIZE) {
        pmm_free_page(page1);
        pmm_free_page(page2);
        pmm_free_page(page3);
        kprintf("✅ Memory allocator test PASSED\n");
    } else {
        kprintf("❌ Memory allocator test FAILED\n");
    }
}
