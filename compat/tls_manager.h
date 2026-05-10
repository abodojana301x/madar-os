#ifndef COMPAT_TLS_MANAGER_H
#define COMPAT_TLS_MANAGER_H

#include <types.h>
#include "pe_structs.h"
#include "pe_parser.h"

#define COMPAT_TLS_CALLBACK_MAX 16

typedef void (*tls_callback_t)(void *instance, uint32_t reason, void *reserved);

typedef struct {
    pe_tls_directory64_t tls_dir;
    tls_callback_t callbacks[COMPAT_TLS_CALLBACK_MAX];
    uint32_t callback_count;
    int initialized;
} compat_tls_info_t;

void tls_manager_init(void);
int tls_set(uint32_t tid, uint32_t slot, uint64_t value);
int tls_get(uint32_t tid, uint32_t slot, uint64_t *value_out);
int tls_parse_directory(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info);
int tls_execute_callbacks(uint32_t reason);
void tls_print_info(void);

#endif
