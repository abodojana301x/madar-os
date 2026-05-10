#include "mem_mapper.h"
#include <heap.h>
#include <string.h>
#include <kprintf.h>

static compat_mem_region_t g_regions[COMPAT_MAX_MEM_REGIONS];
static int g_mem_initialized = 0;

void compat_mem_init(void)
{
    if (g_mem_initialized) return;
    memset(g_regions, 0, sizeof(g_regions));
    g_mem_initialized = 1;
}

static int find_free_slot(void)
{
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (!g_regions[i].used) return i;
    }
    return -1;
}

void *compat_mmap_region(uint32_t size, uint32_t prot)
{
    if (!g_mem_initialized) compat_mem_init();
    if (size == 0) return NULL;

    int slot = find_free_slot();
    if (slot < 0) return NULL;

    void *mem = kmalloc(size);
    if (!mem) return NULL;

    memset(mem, 0, size);

    g_regions[slot].base = mem;
    g_regions[slot].size = size;
    g_regions[slot].prot = prot;
    g_regions[slot].used = 1;

    return mem;
}

void compat_munmap_region(void *addr)
{
    if (!addr) return;
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (g_regions[i].used && g_regions[i].base == addr) {
            kfree(addr);
            memset(&g_regions[i], 0, sizeof(g_regions[i]));
            return;
        }
    }
    kfree(addr);
}

int compat_protect_region(void *addr, uint32_t size, uint32_t prot)
{
    (void)size;
    if (!addr) return -1;
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (g_regions[i].used && g_regions[i].base == addr) {
            g_regions[i].prot = prot;
            return 0;
        }
    }
    return -1;
}

void compat_mem_list_regions(void)
{
    kprintf("[compat-mem] tracked regions:\n");
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (g_regions[i].used) {
            kprintf("  [%d] base=%p size=%u prot=%c%c%c\n",
                    i, g_regions[i].base, g_regions[i].size,
                    (g_regions[i].prot & COMPAT_PROT_READ) ? 'R' : '-',
                    (g_regions[i].prot & COMPAT_PROT_WRITE) ? 'W' : '-',
                    (g_regions[i].prot & COMPAT_PROT_EXEC) ? 'X' : '-');
        }
    }
}

compat_mem_region_t *compat_mem_find_region(void *addr)
{
    if (!addr) return NULL;
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (g_regions[i].used && g_regions[i].base == addr) {
            return &g_regions[i];
        }
    }
    return NULL;
}

uint32_t compat_mem_total_used(void)
{
    uint32_t total = 0;
    for (int i = 0; i < COMPAT_MAX_MEM_REGIONS; i++) {
        if (g_regions[i].used) total += g_regions[i].size;
    }
    return total;
}
