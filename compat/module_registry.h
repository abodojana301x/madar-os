#ifndef COMPAT_MODULE_REGISTRY_H
#define COMPAT_MODULE_REGISTRY_H

#include <types.h>
#include "pe_parser.h"

#define COMPAT_MAX_MODULES 32
#define COMPAT_MAX_EXPORTS 512
#define COMPAT_MAX_DLL_NAME 64
#define COMPAT_MAX_FUNC_NAME 256

typedef struct {
    char name[COMPAT_MAX_FUNC_NAME];
    uint16_t ordinal;
    uint64_t address;
    int used;
} compat_export_entry_t;

typedef struct {
    char dll_name[COMPAT_MAX_DLL_NAME];
    compat_export_entry_t exports[COMPAT_MAX_EXPORTS];
    uint32_t export_count;
    uint64_t base_address;
    int loaded;
} compat_module_t;

void module_registry_init(void);
int module_registry_register(const char *dll_name, uint64_t base_address);
int module_registry_add_export(const char *dll_name, const char *func_name, uint16_t ordinal, uint64_t address);
int module_registry_lookup(const char *dll_name, const char *func_name, uint16_t ordinal, uint64_t *addr_out);
int module_registry_load_stubs(void);
void module_registry_print(void);

extern uint64_t compat_ntdll_stub(const char *name);
extern uint64_t compat_kernel32_stub(const char *name);

#endif
