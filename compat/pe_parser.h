#ifndef COMPAT_PE_PARSER_H
#define COMPAT_PE_PARSER_H

#include <types.h>
#include "pe_structs.h"

typedef struct {
    char name[9];
    uint32_t rva;
    uint32_t virtual_size;
    uint32_t raw_size;
    uint32_t raw_offset;
    uint32_t characteristics;
} pe_section_info_t;

typedef struct {
    uint32_t rva;
    uint32_t size;
} pe_directory_entry_t;

typedef struct {
    uint32_t rva;
    uint32_t name_rva;
    uint32_t ordinal_hint;
    int by_ordinal;
} pe_import_symbol_t;

typedef struct {
    uint32_t rva;
    uint32_t block_size;
} pe_reloc_block_t;

typedef struct {
    int valid;
    uint64_t image_base;
    uint64_t entry_point;
    uint32_t size_of_image;
    uint32_t size_of_headers;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t section_count;
    uint16_t subsystem;
    uint64_t size_of_stack_reserve;
    uint64_t size_of_stack_commit;
    pe_section_info_t sections[PE_MAX_SECTIONS];
    pe_directory_entry_t data_dirs[PE_MAX_DATA_DIRECTORIES];
    int has_imports;
    int has_exports;
    int has_relocs;
    int has_tls;
} pe_image_info_t;

int pe_parse_image(const uint8_t *file_data, uint32_t file_size, pe_image_info_t *out);
int pe_validate_image(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info);

int pe_rva_to_offset(const pe_image_info_t *info, uint32_t rva, uint32_t *offset_out);
const char *pe_get_directory_name(int dir_index);
void pe_print_image_info(const pe_image_info_t *info);

#endif
