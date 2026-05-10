#include "pe_validator.h"
#include <kprintf.h>
#include <string.h>

int pe_validate_headers(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info)
{
    return pe_validate_image(file_data, file_size, info);
}

int pe_validate_sections(const pe_image_info_t *info)
{
    if (!info || !info->valid) return -1;
    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];
        if (s->virtual_size == 0 && s->raw_size == 0) return -1;
    }
    return 0;
}

int pe_validate_full(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_validation_result_t *result)
{
    if (!result) return -1;
    memset(result, 0, sizeof(*result));

    if (!file_data || !info) {
        result->error_msg = "null pointer";
        return -1;
    }
    if (!info->valid) {
        result->error_msg = "image info not valid";
        return -1;
    }

    result->header_ok = 1;
    result->section_table_ok = 1;
    result->alignment_ok = 1;
    result->valid_machine = 1;
    result->valid_opt_magic = 1;
    result->valid_subsystem = 1;
    result->image_size_ok = 1;
    result->entry_point_ok = 1;

    if (info->size_of_headers > file_size) {
        result->header_ok = 0;
        result->error_msg = "size_of_headers exceeds file size";
    }

    if (info->size_of_headers < sizeof(pe_dos_header_t) + 4 + sizeof(pe_coff_header_t) + sizeof(pe_optional_header64_t)) {
        result->header_ok = 0;
        result->error_msg = "size_of_headers too small";
    }

    if (info->size_of_headers > info->size_of_image) {
        result->alignment_ok = 0;
        result->error_msg = "headers exceed image size";
    }

    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];
        if (s->rva & (info->section_alignment - 1)) {
            result->alignment_ok = 0;
            result->error_msg = "section RVA not aligned";
        }
        for (uint16_t j = 0; j < i; j++) {
            const pe_section_info_t *s2 = &info->sections[j];
            uint32_t s2_end = s2->rva + s2->virtual_size;
            uint32_t s_end = s->rva + s->virtual_size;
            if (s->rva < s2_end && s2->rva < s_end) {
                result->overlapping_sections = 1;
                result->error_msg = "overlapping sections";
            }
        }
        if (s->virtual_size > 0 && s->rva + s->virtual_size > info->size_of_image) {
            result->image_size_ok = 0;
            result->error_msg = "section extends past image";
        }
    }

    if (info->entry_point < info->image_base) {
        result->entry_point_ok = 0;
        result->error_msg = "entry point below image base";
    }

    if (info->subsystem != PE_SUBSYSTEM_CONSOLE && info->subsystem != PE_SUBSYSTEM_GUI) {
        result->valid_subsystem = 0;
        result->error_msg = "unrecognized subsystem";
    }

    return (result->header_ok && result->section_table_ok && result->alignment_ok &&
            !result->overlapping_sections && result->image_size_ok && result->entry_point_ok) ? 0 : -1;
}

void pe_print_validation(const pe_validation_result_t *result)
{
    if (!result) return;
    kprintf("[pe-val] header=%s sections=%s align=%s overlap=%s machine=%s subsys=%s imagesz=%s entry=%s\n",
            result->header_ok ? "OK" : "FAIL",
            result->section_table_ok ? "OK" : "FAIL",
            result->alignment_ok ? "OK" : "FAIL",
            result->overlapping_sections ? "YES" : "NO",
            result->valid_machine ? "OK" : "FAIL",
            result->valid_subsystem ? "OK" : "FAIL",
            result->image_size_ok ? "OK" : "FAIL",
            result->entry_point_ok ? "OK" : "FAIL");
    if (result->error_msg) {
        kprintf("[pe-val] error: %s\n", result->error_msg);
    }
}
