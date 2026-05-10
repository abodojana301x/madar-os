#include "safe_exit.h"
#include <kprintf.h>
#include <string.h>

#define COMPAT_MAX_ATEXIT 32
#define COMPAT_MAX_CLEANUP_PATHS 16

static compat_atexit_func_t g_atexit_funcs[COMPAT_MAX_ATEXIT];
static uint32_t g_atexit_count = 0;
static char g_cleanup_paths[COMPAT_MAX_CLEANUP_PATHS][64];
static uint32_t g_cleanup_path_count = 0;

int compat_atexit(compat_atexit_func_t func)
{
    if (!func || g_atexit_count >= COMPAT_MAX_ATEXIT) return -1;
    g_atexit_funcs[g_atexit_count++] = func;
    return 0;
}

int compat_register_cleanup_path(const char *path)
{
    if (!path || g_cleanup_path_count >= COMPAT_MAX_CLEANUP_PATHS) return -1;
    strncpy(g_cleanup_paths[g_cleanup_path_count], path, 63);
    g_cleanup_paths[g_cleanup_path_count][63] = '\0';
    g_cleanup_path_count++;
    return 0;
}

void compat_cleanup_on_crash(void)
{
    kprintf("[compat-safe] cleanup_on_crash: %u atexit funcs, %u paths\n",
            g_atexit_count, g_cleanup_path_count);
    for (uint32_t i = g_atexit_count; i > 0; i--) {
        if (g_atexit_funcs[i - 1]) {
            g_atexit_funcs[i - 1]();
        }
    }
}

void compat_auto_close_on_exit(void)
{
    kprintf("[compat-safe] auto_close_on_exit done\n");
}

void compat_exit_cleanup(uint32_t exit_code)
{
    kprintf("[compat-safe] exit cleanup (code=%u)\n", exit_code);
    compat_cleanup_on_crash();
    compat_auto_close_on_exit();
}
