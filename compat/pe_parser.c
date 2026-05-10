#include "pe_parser.h"
#include <string.h>
#include <kprintf.h>

static int safe_range(uint32_t off, uint32_t size, uint32_t file_size)
{
    if (off > file_size) return 0;
    if (size > file_size - off) return 0;
    return 1;
}

int pe_rva_to_offset(const pe_image_info_t *info, uint32_t rva, uint32_t *offset_out)
{
    if (!info || !offset_out) return -1;
    if (rva < info->size_of_headers) {
        *offset_out = rva;
        return 0;
    }
    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];
        uint32_t sec_start = s->rva;
        uint32_t sec_end = s->rva + s->virtual_size;
        if (s->raw_size > 0 && rva >= sec_start && rva < sec_end) {
            *offset_out = s->raw_offset + (rva - sec_start);
            return 0;
        }
    }
    return -1;
}

const char *pe_get_directory_name(int dir_index)
{
    switch (dir_index) {
        case 0:  return "EXPORT";
        case 1:  return "IMPORT";
        case 2:  return "RESOURCE";
        case 3:  return "EXCEPTION";
        case 4:  return "SECURITY";
        case 5:  return "BASERELOC";
        case 6:  return "DEBUG";
        case 7:  return "ARCHITECTURE";
        case 8:  return "GLOBALPTR";
        case 9:  return "TLS";
        case 10: return "LOAD_CONFIG";
        case 11: return "BOUND_IMPORT";
        case 12: return "IAT";
        case 13: return "DELAY_IMPORT";
        case 14: return "COM_DESCRIPTOR";
        default: return "UNKNOWN";
    }
}

int pe_parse_image(const uint8_t *file_data, uint32_t file_size, pe_image_info_t *out)
{
    if (!file_data || !out || file_size < sizeof(pe_dos_header_t)) return -1;
    memset(out, 0, sizeof(*out));

    const pe_dos_header_t *dos = (const pe_dos_header_t *)file_data;
    if (dos->e_magic != PE_DOS_MAGIC) return -1;
    if (!safe_range(dos->e_lfanew, 4 + sizeof(pe_coff_header_t), file_size)) return -1;

    const uint32_t *sig = (const uint32_t *)(file_data + dos->e_lfanew);
    if (*sig != PE_NT_SIGNATURE) return -1;

    const pe_coff_header_t *coff = (const pe_coff_header_t *)(file_data + dos->e_lfanew + 4);
    if (coff->machine != PE_MACHINE_AMD64) return -1;
    if (coff->number_of_sections == 0 || coff->number_of_sections > PE_MAX_SECTIONS) return -1;

    uint32_t opt_off = dos->e_lfanew + 4 + sizeof(pe_coff_header_t);
    if (!safe_range(opt_off, coff->size_of_optional_header, file_size)) return -1;
    if (coff->size_of_optional_header < sizeof(pe_optional_header64_t)) return -1;

    const pe_optional_header64_t *opt = (const pe_optional_header64_t *)(file_data + opt_off);
    if (opt->magic != PE_OPT_MAGIC_PE32_PLUS) return -1;

    if (opt->section_alignment < 512 || opt->file_alignment < 512) return -1;
    if ((opt->size_of_image & (opt->section_alignment - 1)) != 0) return -1;
    if (opt->size_of_image == 0) return -1;

    uint32_t sec_off = opt_off + coff->size_of_optional_header;
    uint32_t sec_tbl_size = (uint32_t)coff->number_of_sections * sizeof(pe_section_header_t);
    if (!safe_range(sec_off, sec_tbl_size, file_size)) return -1;

    out->valid = 1;
    out->image_base = opt->image_base;
    out->entry_point = opt->image_base + opt->address_of_entry_point;
    out->size_of_image = opt->size_of_image;
    out->size_of_headers = opt->size_of_headers;
    out->section_alignment = opt->section_alignment;
    out->file_alignment = opt->file_alignment;
    out->section_count = coff->number_of_sections;
    out->subsystem = opt->subsystem;
    out->size_of_stack_reserve = opt->size_of_stack_reserve;
    out->size_of_stack_commit = opt->size_of_stack_commit;

    const pe_section_header_t *sec = (const pe_section_header_t *)(file_data + sec_off);
    for (uint16_t i = 0; i < out->section_count; i++) {
        for (int j = 0; j < 8; j++) {
            out->sections[i].name[j] = (char)sec[i].name[j];
        }
        out->sections[i].name[8] = '\0';
        out->sections[i].rva = sec[i].virtual_address;
        out->sections[i].virtual_size = sec[i].virtual_size;
        out->sections[i].raw_size = sec[i].size_of_raw_data;
        out->sections[i].raw_offset = sec[i].pointer_to_raw_data;
        out->sections[i].characteristics = sec[i].characteristics;
    }

    uint32_t data_dir_off = opt_off + sizeof(pe_optional_header64_t);
    uint32_t num_dirs = opt->number_of_rva_and_sizes;
    if (num_dirs > PE_MAX_DATA_DIRECTORIES) num_dirs = PE_MAX_DATA_DIRECTORIES;

    if (safe_range(data_dir_off, num_dirs * sizeof(pe_data_directory_t), file_size)) {
        const pe_data_directory_t *dirs = (const pe_data_directory_t *)(file_data + data_dir_off);
        for (uint32_t i = 0; i < num_dirs; i++) {
            out->data_dirs[i].rva = dirs[i].virtual_address;
            out->data_dirs[i].size = dirs[i].size;
        }
    }

    if (out->data_dirs[PE_DIR_ENTRY_IMPORT].rva && out->data_dirs[PE_DIR_ENTRY_IMPORT].size)
        out->has_imports = 1;
    if (out->data_dirs[PE_DIR_ENTRY_EXPORT].rva && out->data_dirs[PE_DIR_ENTRY_EXPORT].size)
        out->has_exports = 1;
    if (out->data_dirs[PE_DIR_ENTRY_BASERELOC].rva && out->data_dirs[PE_DIR_ENTRY_BASERELOC].size)
        out->has_relocs = 1;
    if (out->data_dirs[PE_DIR_ENTRY_TLS].rva && out->data_dirs[PE_DIR_ENTRY_TLS].size)
        out->has_tls = 1;

    return 0;
}

int pe_validate_image(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info)
{
    if (!file_data || !info || !info->valid) return -1;

    if (info->size_of_image == 0 || info->size_of_headers == 0) return -1;
    if (info->entry_point < info->image_base) return -1;

    if (info->size_of_headers > info->size_of_image) return -1;
    if (info->size_of_headers > file_size) return -1;
    if (info->section_count == 0) return -1;

    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];

        if (s->rva & (info->section_alignment - 1)) return -1;

        uint32_t sec_end = s->rva + s->virtual_size;
        if (sec_end > info->size_of_image) return -1;

        if (s->raw_size > 0) {
            if (s->raw_offset >= file_size) return -1;
            if (s->raw_size > file_size - s->raw_offset) return -1;
        }

        for (uint16_t j = 0; j < i; j++) {
            const pe_section_info_t *s2 = &info->sections[j];
            uint32_t s2_end = s2->rva + s2->virtual_size;
            if (s->rva < s2_end && s2->rva < sec_end) return -1;
        }
    }

    return 0;
}

void pe_print_image_info(const pe_image_info_t *info)
{
    if (!info || !info->valid) return;
    kprintf("[pe] image_base=0x%llx entry=0x%llx size=%u\n",
            info->image_base, info->entry_point, info->size_of_image);
    kprintf("[pe] sections=%u subsystem=%u stack_reserve=%llu\n",
            info->section_count, info->subsystem, info->size_of_stack_reserve);
    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];
        kprintf("[pe]   [%u] %s va=0x%x vsize=%u raw=%u@0x%x chars=0x%x\n",
                i, s->name, s->rva, s->virtual_size, s->raw_size, s->raw_offset, s->characteristics);
    }
    for (int i = 0; i < PE_MAX_DATA_DIRECTORIES; i++) {
        if (info->data_dirs[i].rva && info->data_dirs[i].size) {
            kprintf("[pe]   dir[%d] %s rva=0x%x size=%u\n",
                    i, pe_get_directory_name(i), info->data_dirs[i].rva, info->data_dirs[i].size);
        }
    }
}
