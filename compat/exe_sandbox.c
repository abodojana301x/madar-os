#include "exe_sandbox.h"
#include <string.h>
#include <kprintf.h>

void sandbox_init(compat_sandbox_t *sb)
{
    if (!sb) return;
    sb->max_memory_mb = COMPAT_SANDBOX_DEFAULT_MEM_MB;
    sb->max_cpu_percent = COMPAT_SANDBOX_DEFAULT_CPU_PCT;
    sb->deny_kernel_paths = 1;
    sb->deny_exec = 1;
    sb->audit_enabled = 1;
    sb->whitelist_count = 0;
    memset(sb->whitelist_paths, 0, sizeof(sb->whitelist_paths));
}

int sandbox_check_path(const compat_sandbox_t *sb, const char *path)
{
    if (!sb || !path) return -1;

    for (uint32_t i = 0; i < sb->whitelist_count; i++) {
        if (strncmp(path, sb->whitelist_paths[i], strlen(sb->whitelist_paths[i])) == 0) {
            return 0;
        }
    }

    if (!sb->deny_kernel_paths) return 0;
    if (strncmp(path, "/kernel", 7) == 0) return -1;
    if (strncmp(path, "/etc", 4) == 0) return -1;
    if (strncmp(path, "/boot", 5) == 0) return -1;
    if (strncmp(path, "/system", 7) == 0 && sb->deny_exec) return -1;
    return 0;
}

int sandbox_check_memory(const compat_sandbox_t *sb, uint32_t mem_mb)
{
    if (!sb) return -1;
    return (mem_mb <= sb->max_memory_mb) ? 0 : -1;
}

int sandbox_add_whitelist_path(compat_sandbox_t *sb, const char *path)
{
    if (!sb || !path || sb->whitelist_count >= 8) return -1;
    strncpy(sb->whitelist_paths[sb->whitelist_count], path, 63);
    sb->whitelist_paths[sb->whitelist_count][63] = '\0';
    sb->whitelist_count++;
    return 0;
}

void sandbox_stats_init(compat_sandbox_stats_t *stats)
{
    if (!stats) return;
    memset(stats, 0, sizeof(*stats));
}

void sandbox_stats_record_path_denial(compat_sandbox_stats_t *stats)
{
    if (stats) stats->path_denials++;
}

void sandbox_stats_print(const compat_sandbox_t *sb, const compat_sandbox_stats_t *stats)
{
    if (!sb) return;
    kprintf("[compat-sandbox] config: max_mem=%uMB max_cpu=%u%% deny_kernel=%d\n",
            sb->max_memory_mb, sb->max_cpu_percent, sb->deny_kernel_paths);
    if (stats) {
        kprintf("[compat-sandbox] stats: peak_mem=%uMB path_denials=%u mem_violations=%u\n",
                stats->peak_mem_mb, stats->path_denials, stats->memory_violations);
    }
}
