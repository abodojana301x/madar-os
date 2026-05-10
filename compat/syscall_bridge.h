#ifndef COMPAT_SYSCALL_BRIDGE_H
#define COMPAT_SYSCALL_BRIDGE_H

#include <types.h>

typedef uint64_t (*compat_api_func_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

typedef struct {
    const char *name;
    compat_api_func_t func;
} compat_api_entry_t;

void compat_syscall_bridge_init(void);
int compat_syscall_bridge_register(const char *name, compat_api_func_t func);
uint64_t compat_call_api_by_name(const char *name, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4);
void compat_syscall_bridge_print(void);

#endif
