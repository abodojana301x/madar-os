#ifndef COMPAT_IMPORT_RESOLVER_H
#define COMPAT_IMPORT_RESOLVER_H

#include <types.h>
#include "pe_parser.h"

#define COMPAT_MAX_IMPORTS 256
#define COMPAT_MAX_DLL_NAME 64
#define COMPAT_MAX_FUNC_NAME 256

typedef struct {
    char dll_name[COMPAT_MAX_DLL_NAME];
    char func_name[COMPAT_MAX_FUNC_NAME];
    uint16_t ordinal;
    int by_ordinal;
    uint64_t resolved_addr;
    int resolved;
} compat_import_entry_t;

typedef struct {
    compat_import_entry_t entries[COMPAT_MAX_IMPORTS];
    uint32_t count;
} compat_import_table_t;

void import_resolver_init(void);
int import_resolver_parse(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, compat_import_table_t *table);
int import_resolver_resolve(compat_import_table_t *table, uint8_t *image_base, const pe_image_info_t *info);
int import_resolver_lazy_resolve(const char *dll, const char *func, uint64_t *addr_out);
void import_resolver_print(const compat_import_table_t *table);

#endif
