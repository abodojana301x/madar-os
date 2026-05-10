#ifndef COMPAT_EXE_SANDBOX_H
#define COMPAT_EXE_SANDBOX_H

#include <types.h>

#define COMPAT_SANDBOX_DEFAULT_MEM_MB  256
#define COMPAT_SANDBOX_DEFAULT_CPU_PCT 50

typedef struct {
    uint32_t max_memory_mb;
    uint32_t max_cpu_percent;
    int deny_kernel_paths;
    int deny_exec;
    int audit_enabled;
    char whitelist_paths[8][64];
    uint32_t whitelist_count;
} compat_sandbox_t;

typedef struct {
    uint32_t total_mem_allocated_mb;
    uint32_t peak_mem_mb;
    uint32_t total_cpu_ticks;
    uint32_t path_denials;
    uint32_t memory_violations;
} compat_sandbox_stats_t;

void sandbox_init(compat_sandbox_t *sb);
int sandbox_check_path(const compat_sandbox_t *sb, const char *path);
int sandbox_check_memory(const compat_sandbox_t *sb, uint32_t mem_mb);
int sandbox_add_whitelist_path(compat_sandbox_t *sb, const char *path);
void sandbox_stats_init(compat_sandbox_stats_t *stats);
void sandbox_stats_record_path_denial(compat_sandbox_stats_t *stats);
void sandbox_stats_print(const compat_sandbox_t *sb, const compat_sandbox_stats_t *stats);

#endif
