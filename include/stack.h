/*
 * Layer: 2 Stack interface
 */

#ifndef STACK_H
#define STACK_H

#include <types.h>

uint64_t *kernel_stack_alloc(void);
void kernel_stack_free(uint64_t *stack);

#endif
