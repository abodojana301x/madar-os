#include "stack_init.h"
#include "mem_mapper.h"
#include <kprintf.h>

int compat_stack_prepare(uint8_t *stack_base, uint32_t stack_size, uint64_t *rsp_out)
{
    if (!stack_base || stack_size < 64 || !rsp_out) return -1;
    uint64_t sp = (uint64_t)(uintptr_t)(stack_base + stack_size);
    sp &= ~0xFULL;
    *rsp_out = sp;
    return 0;
}

int compat_stack_allocate(uint32_t size, uint8_t **base_out, uint64_t *rsp_out)
{
    if (!base_out || !rsp_out) return -1;
    if (size == 0) size = COMPAT_DEFAULT_STACK_SIZE;

    uint8_t *stack = (uint8_t *)compat_mmap_region(size, 3);
    if (!stack) {
        kprintf("[compat-stack] failed to allocate %u byte stack\n", size);
        return -1;
    }

    *base_out = stack;
    uint64_t sp = (uint64_t)(uintptr_t)(stack + size);
    sp &= ~0xFULL;
    *rsp_out = sp;
    return 0;
}

void compat_stack_free(uint8_t *base)
{
    if (base) compat_munmap_region(base);
}
