#ifndef COMPAT_MEM_MAPPER_H
#define COMPAT_MEM_MAPPER_H

#include <types.h>

#define COMPAT_PROT_READ   0x1
#define COMPAT_PROT_WRITE  0x2
#define COMPAT_PROT_EXEC   0x4

#define COMPAT_ALLOC_TYPE_RESERVE 0x2000
#define COMPAT_ALLOC_TYPE_COMMIT  0x1000

#define COMPAT_MAX_MEM_REGIONS 64

typedef struct {
    void *base;
    uint32_t size;
    uint32_t prot;
    int used;
} compat_mem_region_t;

void compat_mem_init(void);
void *compat_mmap_region(uint32_t size, uint32_t prot);
void compat_munmap_region(void *addr);
int compat_protect_region(void *addr, uint32_t size, uint32_t prot);
void compat_mem_list_regions(void);
compat_mem_region_t *compat_mem_find_region(void *addr);
uint32_t compat_mem_total_used(void);

#endif
