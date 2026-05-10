#include "import_resolver.h"
#include "pe_structs.h"
#include "module_registry.h"
#include <string.h>
#include <kprintf.h>

static int g_ir_init = 0;
static compat_import_table_t g_import_table;

void import_resolver_init(void)
{
    if (g_ir_init) return;
    memset(&g_import_table, 0, sizeof(g_import_table));
    g_ir_init = 1;
}

int import_resolver_parse(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, compat_import_table_t *table)
{
    if (!file_data || !info || !table) return -1;
    memset(table, 0, sizeof(*table));

    if (!info->has_imports) return 0;

    uint32_t import_rva = info->data_dirs[PE_DIR_ENTRY_IMPORT].rva;
    uint32_t import_size = info->data_dirs[PE_DIR_ENTRY_IMPORT].size;
    if (import_rva == 0 || import_size == 0) return 0;

    uint32_t import_off = 0;
    if (pe_rva_to_offset(info, import_rva, &import_off) != 0) return -1;

    uint32_t pos = import_off;
    uint32_t end = import_off + import_size;

    while (pos + sizeof(pe_import_descriptor_t) <= end) {
        const pe_import_descriptor_t *desc = (const pe_import_descriptor_t *)(file_data + pos);
        if (desc->import_lookup_table_rva == 0 && desc->name_rva == 0) break;

        char dll_name[COMPAT_MAX_DLL_NAME] = {0};
        if (desc->name_rva) {
            uint32_t name_off = 0;
            if (pe_rva_to_offset(info, desc->name_rva, &name_off) == 0) {
                strncpy(dll_name, (const char *)(file_data + name_off), COMPAT_MAX_DLL_NAME - 1);
            }
        }

        uint32_t ilt_rva = desc->import_lookup_table_rva;
        if (ilt_rva == 0) ilt_rva = desc->import_address_table_rva;

        uint32_t ilt_off = 0;
        if (pe_rva_to_offset(info, ilt_rva, &ilt_off) != 0) {
            pos += sizeof(pe_import_descriptor_t);
            continue;
        }

        for (uint32_t slot = 0; slot < 4096; slot++) {
            uint64_t import_entry = 0;
            if (ilt_off + 8 > file_size) break;
            import_entry = *(const uint64_t *)(file_data + ilt_off + slot * 8);
            if (import_entry == 0) break;

            if (table->count >= COMPAT_MAX_IMPORTS) break;

            compat_import_entry_t *ie = &table->entries[table->count];
            strncpy(ie->dll_name, dll_name, COMPAT_MAX_DLL_NAME - 1);

            if (import_entry & (1ULL << 63)) {
                ie->by_ordinal = 1;
                ie->ordinal = (uint16_t)(import_entry & 0xFFFF);
            } else {
                ie->by_ordinal = 0;
                uint32_t hint_name_rva = (uint32_t)import_entry;
                uint32_t hn_off = 0;
                if (pe_rva_to_offset(info, hint_name_rva, &hn_off) == 0) {
                    uint16_t hint = *(const uint16_t *)(file_data + hn_off);
                    (void)hint;
                    const char *fname = (const char *)(file_data + hn_off + 2);
                    strncpy(ie->func_name, fname, COMPAT_MAX_FUNC_NAME - 1);
                }
            }

            ie->resolved_addr = 0;
            ie->resolved = 0;
            table->count++;
        }

        pos += sizeof(pe_import_descriptor_t);
    }

    kprintf("[compat-import] parsed %u imports from %u descriptors\n", table->count,
            (uint32_t)((pos - import_off) / sizeof(pe_import_descriptor_t)));
    return 0;
}

int import_resolver_resolve(compat_import_table_t *table, uint8_t *image_base, const pe_image_info_t *info)
{
    if (!table || !image_base || !info) return -1;
    uint32_t resolved = 0;
    uint32_t failed = 0;

    for (uint32_t i = 0; i < table->count; i++) {
        compat_import_entry_t *ie = &table->entries[i];

        uint64_t addr = 0;
        if (module_registry_lookup(ie->dll_name, ie->by_ordinal ? NULL : ie->func_name,
                                    ie->by_ordinal ? ie->ordinal : 0, &addr) == 0) {
            ie->resolved_addr = addr;
            ie->resolved = 1;
            resolved++;
        } else {
            failed++;
            kprintf("[compat-import] WARNING: unresolved %s!%s%s\n",
                    ie->dll_name,
                    ie->by_ordinal ? "#" : "",
                    ie->by_ordinal ? "" : ie->func_name);
        }
    }

    kprintf("[compat-import] resolved %u/%u imports (%u failed)\n",
            resolved, table->count, failed);
    return (failed == 0) ? 0 : -1;
}

int import_resolver_lazy_resolve(const char *dll, const char *func, uint64_t *addr_out)
{
    if (!addr_out) return -1;
    return module_registry_lookup(dll, func, 0, addr_out);
}

void import_resolver_print(const compat_import_table_t *table)
{
    if (!table) return;
    kprintf("[compat-import] import table (%u entries):\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        const compat_import_entry_t *ie = &table->entries[i];
        kprintf("  [%u] %s", i, ie->dll_name);
        if (ie->by_ordinal) {
            kprintf(" ordinal=%u", ie->ordinal);
        } else {
            kprintf(" %s", ie->func_name);
        }
        kprintf(" => %s (0x%llx)\n", ie->resolved ? "OK" : "UNRESOLVED", ie->resolved_addr);
    }
}
