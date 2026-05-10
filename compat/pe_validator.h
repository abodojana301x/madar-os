#ifndef COMPAT_PE_VALIDATOR_H
#define COMPAT_PE_VALIDATOR_H

#include <types.h>
#include "pe_parser.h"

typedef struct {
    int header_ok;
    int section_table_ok;
    int alignment_ok;
    int overlapping_sections;
    int valid_machine;
    int valid_opt_magic;
    int valid_subsystem;
    int image_size_ok;
    int entry_point_ok;
    const char *error_msg;
} pe_validation_result_t;

int pe_validate_headers(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info);
int pe_validate_sections(const pe_image_info_t *info);
int pe_validate_full(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_validation_result_t *result);
void pe_print_validation(const pe_validation_result_t *result);

#endif
