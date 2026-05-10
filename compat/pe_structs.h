#ifndef COMPAT_PE_STRUCTS_H
#define COMPAT_PE_STRUCTS_H

#include <types.h>

#define PE_DOS_MAGIC            0x5A4D
#define PE_NT_SIGNATURE         0x00004550
#define PE_OPT_MAGIC_PE32_PLUS  0x20B
#define PE_MAX_SECTIONS         16
#define PE_MAX_DATA_DIRECTORIES 16

#define PE_MACHINE_AMD64        0x8664

#define PE_SECT_CODE_EXECUTE    0x20000000
#define PE_SECT_EXECUTE         0x20000000
#define PE_SECT_READ            0x40000000
#define PE_SECT_WRITE           0x80000000
#define PE_SECT_CODE            0x00000020
#define PE_SECT_INIT_DATA       0x00000040
#define PE_SECT_UNINIT_DATA     0x00000080

#define PE_DIR_ENTRY_EXPORT         0
#define PE_DIR_ENTRY_IMPORT         1
#define PE_DIR_ENTRY_RESOURCE       2
#define PE_DIR_ENTRY_EXCEPTION      3
#define PE_DIR_ENTRY_SECURITY       4
#define PE_DIR_ENTRY_BASERELOC      5
#define PE_DIR_ENTRY_DEBUG          6
#define PE_DIR_ENTRY_TLS            9
#define PE_DIR_ENTRY_IAT            12

#define PE_SUBSYSTEM_CONSOLE    3
#define PE_SUBSYSTEM_GUI        2

#define PE_REL_BASED_ABSOLUTE   0
#define PE_REL_BASED_HIGH       1
#define PE_REL_BASED_LOW        2
#define PE_REL_BASED_HIGHLOW    3
#define PE_REL_BASED_DIR64      10

typedef struct __attribute__((packed)) {
    uint16_t e_magic;
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;
} pe_dos_header_t;

typedef struct __attribute__((packed)) {
    uint16_t machine;
    uint16_t number_of_sections;
    uint32_t time_date_stamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbols;
    uint16_t size_of_optional_header;
    uint16_t characteristics;
} pe_coff_header_t;

typedef struct __attribute__((packed)) {
    uint32_t virtual_address;
    uint32_t size;
} pe_data_directory_t;

typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint8_t  major_linker_version;
    uint8_t  minor_linker_version;
    uint32_t size_of_code;
    uint32_t size_of_initialized_data;
    uint32_t size_of_uninitialized_data;
    uint32_t address_of_entry_point;
    uint32_t base_of_code;
    uint64_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_os_version;
    uint16_t minor_os_version;
    uint16_t major_image_version;
    uint16_t minor_image_version;
    uint16_t major_subsystem_version;
    uint16_t minor_subsystem_version;
    uint32_t win32_version_value;
    uint32_t size_of_image;
    uint32_t size_of_headers;
    uint32_t check_sum;
    uint16_t subsystem;
    uint16_t dll_characteristics;
    uint64_t size_of_stack_reserve;
    uint64_t size_of_stack_commit;
    uint64_t size_of_heap_reserve;
    uint64_t size_of_heap_commit;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
} pe_optional_header64_t;

typedef struct __attribute__((packed)) {
    uint8_t  name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t size_of_raw_data;
    uint32_t pointer_to_raw_data;
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_linenumbers;
    uint16_t number_of_relocations;
    uint16_t number_of_linenumbers;
    uint32_t characteristics;
} pe_section_header_t;

typedef struct __attribute__((packed)) {
    uint32_t import_lookup_table_rva;
    uint32_t time_date_stamp;
    uint32_t forwarder_chain;
    uint32_t name_rva;
    uint32_t import_address_table_rva;
} pe_import_descriptor_t;

typedef struct __attribute__((packed)) {
    uint32_t characteristics;
    uint32_t time_date_stamp;
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t name_rva;
    uint32_t ordinal_base;
    uint32_t number_of_functions;
    uint32_t number_of_names;
    uint32_t address_of_functions;
    uint32_t address_of_names;
    uint32_t address_of_name_ordinals;
} pe_export_directory_t;

typedef struct __attribute__((packed)) {
    uint32_t page_rva;
    uint32_t block_size;
} pe_base_reloc_block_t;

typedef struct __attribute__((packed)) {
    uint32_t raw_data_start_va;
    uint32_t raw_data_end_va;
    uint32_t address_of_index;
    uint32_t address_of_callbacks;
    uint32_t size_of_zero_fill;
    uint32_t characteristics;
} pe_tls_directory64_t;

#endif
