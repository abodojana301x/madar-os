#include "tls_manager.h"
#include "pe_threads.h"
#include "pe_structs.h"
#include <kprintf.h>
#include <string.h>
#include <heap.h>

static compat_tls_info_t g_tls;

void tls_manager_init(void)
{
    memset(&g_tls, 0, sizeof(g_tls));
    g_tls.initialized = 1;
    compat_threads_init();
    kprintf("[compat-tls] TLS manager initialized\n");
}

int tls_set(uint32_t tid, uint32_t slot, uint64_t value)
{
    return compat_set_tls(tid, slot, value);
}

int tls_get(uint32_t tid, uint32_t slot, uint64_t *value_out)
{
    return compat_get_tls(tid, slot, value_out);
}

int tls_parse_directory(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info)
{
    if (!file_data || !info || !info->has_tls) return -1;

    uint32_t tls_rva = info->data_dirs[PE_DIR_ENTRY_TLS].rva;
    uint32_t tls_size = info->data_dirs[PE_DIR_ENTRY_TLS].size;
    if (tls_rva == 0 || tls_size < sizeof(pe_tls_directory64_t)) return -1;

    uint32_t tls_off = 0;
    if (pe_rva_to_offset(info, tls_rva, &tls_off) != 0) return -1;
    if (tls_off + sizeof(pe_tls_directory64_t) > file_size) return -1;

    const pe_tls_directory64_t *tls_dir = (const pe_tls_directory64_t *)(file_data + tls_off);
    g_tls.tls_dir = *tls_dir;

    if (tls_dir->address_of_callbacks != 0) {
        uint32_t callbacks_rva = (uint32_t)(tls_dir->address_of_callbacks - info->image_base);
        uint32_t cb_off = 0;
        if (pe_rva_to_offset(info, callbacks_rva, &cb_off) == 0) {
            g_tls.callback_count = 0;
            while (g_tls.callback_count < COMPAT_TLS_CALLBACK_MAX) {
                uint64_t cb_addr;
                if (cb_off + 8 > file_size) break;
                cb_addr = *(const uint64_t *)(file_data + cb_off);
                if (cb_addr == 0) break;
                g_tls.callbacks[g_tls.callback_count] = (tls_callback_t)(uintptr_t)cb_addr;
                g_tls.callback_count++;
                cb_off += 8;
            }
        }
    }

    kprintf("[compat-tls] TLS directory parsed: %u callbacks, data=[0x%x-0x%x]\n",
            g_tls.callback_count, tls_dir->raw_data_start_va, tls_dir->raw_data_end_va);
    return 0;
}

int tls_execute_callbacks(uint32_t reason)
{
    if (!g_tls.initialized) return -1;
    for (uint32_t i = 0; i < g_tls.callback_count; i++) {
        if (g_tls.callbacks[i]) {
            kprintf("[compat-tls] executing callback[%u]=%p reason=%u\n",
                    i, (void *)(uintptr_t)g_tls.callbacks[i], reason);
            g_tls.callbacks[i](NULL, reason, NULL);
        }
    }
    return 0;
}

void tls_print_info(void)
{
    if (!g_tls.initialized) return;
    kprintf("[compat-tls] callbacks=%u\n", g_tls.callback_count);
    kprintf("[compat-tls] raw_data=[0x%x-0x%x] index=0x%x zero_fill=%u\n",
            g_tls.tls_dir.raw_data_start_va, g_tls.tls_dir.raw_data_end_va,
            g_tls.tls_dir.address_of_index, g_tls.tls_dir.size_of_zero_fill);
}
