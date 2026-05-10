#include "syscall_bridge.h"
#include "win32_stubs.h"
#include <string.h>
#include <kprintf.h>

#define COMPAT_MAX_API_ENTRIES 64

static compat_api_entry_t g_api_entries[COMPAT_MAX_API_ENTRIES];
static uint32_t g_api_count = 0;

void compat_syscall_bridge_init(void)
{
    g_api_count = 0;
    memset(g_api_entries, 0, sizeof(g_api_entries));
}

int compat_syscall_bridge_register(const char *name, compat_api_func_t func)
{
    if (!name || !func || g_api_count >= COMPAT_MAX_API_ENTRIES) return -1;
    g_api_entries[g_api_count].name = name;
    g_api_entries[g_api_count].func = func;
    g_api_count++;
    return 0;
}

uint64_t compat_call_api_by_name(const char *name, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4)
{
    if (!name) return (uint64_t)-1;

    for (uint32_t i = 0; i < g_api_count; i++) {
        if (strcmp(name, g_api_entries[i].name) == 0) {
            return g_api_entries[i].func(a1, a2, a3, a4, 0, 0);
        }
    }

#define TRY_API(n) do { if (strcmp(name, #n) == 0) return (uint64_t)w32_##n(); } while(0)
#define TRY_API1(n) do { if (strcmp(name, #n) == 0) { w32_##n((uint32_t)a1); return 0; } } while(0)

    TRY_API(GetTickCount);
    TRY_API(GetCurrentProcess);
    TRY_API(GetCurrentThread);
    TRY_API(GetLastError);
    TRY_API(GetProcessHeap);
    TRY_API1(SetLastError);
    TRY_API1(Sleep);
    TRY_API1(DebugBreak);

    if (strcmp(name, "WaitForSingleObject") == 0)
        return (uint64_t)w32_WaitForSingleObject((HANDLE)a1, (DWORD)a2);
    if (strcmp(name, "MessageBoxA") == 0)
        return (uint64_t)w32_MessageBoxA((void *)a1, (const char *)a2, (const char *)a3, (uint32_t)a4);

    return (uint64_t)-1;
}

void compat_syscall_bridge_print(void)
{
    kprintf("[compat-syscall] registered API entries:\n");
    for (uint32_t i = 0; i < g_api_count; i++) {
        kprintf("  [%u] %s\n", i, g_api_entries[i].name);
    }
}
