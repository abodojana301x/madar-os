#include "module_registry.h"
#include <string.h>
#include <kprintf.h>

static compat_module_t g_modules[COMPAT_MAX_MODULES];
static int g_mr_init = 0;

static char ascii_tolower(char c)
{
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

static int stricmp(const char *a, const char *b)
{
    if (!a || !b) return -1;
    while (*a && *b) {
        char ca = ascii_tolower(*a);
        char cb = ascii_tolower(*b);
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++;
    }
    return (int)ascii_tolower(*a) - (int)ascii_tolower(*b);
}

void module_registry_init(void)
{
    if (g_mr_init) return;
    memset(g_modules, 0, sizeof(g_modules));
    g_mr_init = 1;
    kprintf("[compat-module] module registry initialized\n");
}

static compat_module_t *find_module(const char *dll_name)
{
    if (!dll_name) return NULL;
    for (int i = 0; i < COMPAT_MAX_MODULES; i++) {
        if (g_modules[i].loaded) {
            if (stricmp(g_modules[i].dll_name, dll_name) == 0) return &g_modules[i];
            const char *dot = strchr(g_modules[i].dll_name, '.');
            const char *dot2 = strchr(dll_name, '.');
            if (dot && dot2) {
                size_t len1 = (size_t)(dot - g_modules[i].dll_name);
                size_t len2 = (size_t)(dot2 - dll_name);
                if (len1 == len2 && strncmp(g_modules[i].dll_name, dll_name, len1) == 0) {
                    return &g_modules[i];
                }
            }
        }
    }
    return NULL;
}

static int find_free_module(void)
{
    for (int i = 0; i < COMPAT_MAX_MODULES; i++) {
        if (!g_modules[i].loaded) return i;
    }
    return -1;
}

int module_registry_register(const char *dll_name, uint64_t base_address)
{
    if (!dll_name) return -1;
    compat_module_t *existing = find_module(dll_name);
    if (existing) return 0;

    int slot = find_free_module();
    if (slot < 0) return -1;

    memset(&g_modules[slot], 0, sizeof(g_modules[slot]));
    strncpy(g_modules[slot].dll_name, dll_name, COMPAT_MAX_DLL_NAME - 1);
    g_modules[slot].base_address = base_address;
    g_modules[slot].loaded = 1;
    g_modules[slot].export_count = 0;

    kprintf("[compat-module] registered module: %s @ 0x%llx\n", dll_name, base_address);
    return 0;
}

int module_registry_add_export(const char *dll_name, const char *func_name, uint16_t ordinal, uint64_t address)
{
    compat_module_t *mod = find_module(dll_name);
    if (!mod) return -1;
    if (mod->export_count >= COMPAT_MAX_EXPORTS) return -1;

    compat_export_entry_t *e = &mod->exports[mod->export_count];
    if (func_name) strncpy(e->name, func_name, COMPAT_MAX_FUNC_NAME - 1);
    e->ordinal = ordinal;
    e->address = address;
    e->used = 1;
    mod->export_count++;
    return 0;
}

int module_registry_lookup(const char *dll_name, const char *func_name, uint16_t ordinal, uint64_t *addr_out)
{
    if (!addr_out) return -1;

    compat_module_t *mod = NULL;
    if (dll_name) {
        mod = find_module(dll_name);
        if (mod) {
            for (uint32_t j = 0; j < mod->export_count; j++) {
                compat_export_entry_t *e = &mod->exports[j];
                if (!e->used) continue;
                if (func_name && stricmp(e->name, func_name) == 0) {
                    *addr_out = e->address;
                    return 0;
                }
                if (ordinal && e->ordinal == ordinal) {
                    *addr_out = e->address;
                    return 0;
                }
            }
        }
    }

    for (int i = 0; i < COMPAT_MAX_MODULES; i++) {
        if (!g_modules[i].loaded || (mod && &g_modules[i] == mod)) continue;
        for (uint32_t j = 0; j < g_modules[i].export_count; j++) {
            compat_export_entry_t *e = &g_modules[i].exports[j];
            if (!e->used) continue;
            if (func_name && stricmp(e->name, func_name) == 0) {
                *addr_out = e->address;
                return 0;
            }
            if (ordinal && e->ordinal == ordinal) {
                *addr_out = e->address;
                return 0;
            }
        }
    }
    return -1;
}

int module_registry_load_stubs(void)
{
    module_registry_register("kernel32.dll", 0);
    module_registry_register("ntdll.dll", 0);
    module_registry_register("user32.dll", 0);
    module_registry_register("gdi32.dll", 0);
    module_registry_register("advapi32.dll", 0);
    module_registry_register("shell32.dll", 0);
    return 0;
}

void module_registry_print(void)
{
    kprintf("[compat-module] module registry:\n");
    for (int i = 0; i < COMPAT_MAX_MODULES; i++) {
        if (g_modules[i].loaded) {
            kprintf("  [%d] %s @ 0x%llx exports=%u\n",
                    i, g_modules[i].dll_name, g_modules[i].base_address, g_modules[i].export_count);
            uint32_t show = g_modules[i].export_count;
            if (show > 8) show = 8;
            for (uint32_t j = 0; j < show; j++) {
                kprintf("    [%u] %s ord=%u @ 0x%llx\n",
                        j, g_modules[i].exports[j].name,
                        g_modules[i].exports[j].ordinal,
                        g_modules[i].exports[j].address);
            }
            if (g_modules[i].export_count > 8) {
                kprintf("    ... and %u more\n", g_modules[i].export_count - 8);
            }
        }
    }
}
