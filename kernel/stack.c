/*
 * Layer: 2 Kernel Stack Management
 * Purpose: Allocate and manage per-thread kernel stacks
 */

#include <types.h>
#include <stack.h>
#include <memory.h>
#include <string.h>

#define KERNEL_STACK_PAGES 1  /* 4KB stacks */

uint64_t *kernel_stack_alloc(void)
{
    uint64_t *stack = (uint64_t *)pmm_alloc_page();;
    memset(stack, 0, PAGE_SIZE);
    return stack;
}

void kernel_stack_free(uint64_t *stack)
{
    pmm_free_page((uint64_t)stack);
}
