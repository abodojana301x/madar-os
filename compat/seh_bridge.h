#ifndef COMPAT_SEH_BRIDGE_H
#define COMPAT_SEH_BRIDGE_H

#include <types.h>

#define COMPAT_EX_DIVIDE_BY_ZERO         0xC0000094
#define COMPAT_EX_ACCESS_VIOLATION       0xC0000005
#define COMPAT_EX_ILLEGAL_INSTRUCTION    0xC000001D
#define COMPAT_EX_STACK_OVERFLOW         0xC00000FD
#define COMPAT_EX_INTEGER_OVERFLOW       0xC0000095
#define COMPAT_EX_PRIV_INSTRUCTION       0xC0000096
#define COMPAT_EX_PAGE_FAULT             0xC0000005
#define COMPAT_EX_BREAKPOINT             0xC0000092
#define COMPAT_EX_DATATYPE_MISALIGNMENT  0xC0000235

typedef void (*compat_exception_handler_t)(uint32_t code, uint64_t addr);

typedef struct {
    uint32_t code;
    uint64_t address;
    uint32_t flags;
    uint32_t handled;
} compat_exception_record_t;

void seh_bridge_init(void);
uint32_t seh_map_cpu_exception(uint32_t cpu_vector, uint64_t addr);
void seh_register_handler(compat_exception_handler_t handler);
int seh_raise(uint32_t code, uint64_t addr);
int seh_raise_with_record(compat_exception_record_t *rec);
const char *seh_code_name(uint32_t code);
void seh_print_last(void);

#endif
