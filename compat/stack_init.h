#ifndef COMPAT_STACK_INIT_H
#define COMPAT_STACK_INIT_H

#include <types.h>

#define COMPAT_DEFAULT_STACK_SIZE (1024 * 1024)
#define COMPAT_DEFAULT_STACK_COMMIT (4 * 1024)

int compat_stack_prepare(uint8_t *stack_base, uint32_t stack_size, uint64_t *rsp_out);
int compat_stack_allocate(uint32_t size, uint8_t **base_out, uint64_t *rsp_out);
void compat_stack_free(uint8_t *base);

#endif
