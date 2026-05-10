#ifndef COMPAT_PE_LOADER_H
#define COMPAT_PE_LOADER_H

#include <types.h>
#include "pe_parser.h"
#include "mem_mapper.h"

typedef struct {
    uint8_t *image_base;
    uint64_t entry_point;
    uint32_t mapped_size;
    uint32_t actual_load_base;
    int loaded;
    int relocs_applied;
    compat_mem_region_t *regions;
    uint32_t region_count;
} pe_loaded_image_t;

int pe_load_image(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_loaded_image_t *out);
int pe_apply_relocations(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_loaded_image_t *img);
void pe_unload_image(pe_loaded_image_t *img);
int pe_resolve_imports(pe_loaded_image_t *img, const pe_image_info_t *info, const uint8_t *file_data, uint32_t file_size);

#endif
