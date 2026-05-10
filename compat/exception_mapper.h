#ifndef COMPAT_EXCEPTION_MAPPER_H
#define COMPAT_EXCEPTION_MAPPER_H

#include <types.h>

typedef struct {
    uint32_t cpu_vector;
    uint64_t address;
    uint64_t cr2_value;
} compat_cpu_exception_t;

uint32_t compat_exception_from_vector(uint32_t vector, uint64_t addr);
void compat_exception_handler_dispatch(compat_cpu_exception_t *ctx);

#endif
