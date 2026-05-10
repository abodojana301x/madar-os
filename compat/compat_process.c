#include "compat_process.h"
#include <string.h>
#include <kprintf.h>
#include <timer.h>

static compat_process_t g_processes[COMPAT_MAX_PROCESSES];
static uint64_t g_next_pid = 1;
static int g_proc_init = 0;

void compat_process_init(void)
{
    if (g_proc_init) return;
    memset(g_processes, 0, sizeof(g_processes));
    g_next_pid = 1;
    g_proc_init = 1;
    kprintf("[compat-proc] process subsystem initialized\n");
}

int compat_process_create(const char *name, uint8_t *image_base, uint64_t entry_point, uint32_t mapped_size, uint64_t *pid_out)
{
    if (!name) return -1;
    for (int i = 0; i < COMPAT_MAX_PROCESSES; i++) {
        if (!g_processes[i].used) {
            g_processes[i].used = 1;
            g_processes[i].pid = g_next_pid++;
            strncpy(g_processes[i].name, name, COMPAT_PROCESS_NAME_LEN - 1);
            g_processes[i].name[COMPAT_PROCESS_NAME_LEN - 1] = '\0';
            g_processes[i].image_base = image_base;
            g_processes[i].entry_point = entry_point;
            g_processes[i].mapped_size = mapped_size;
            g_processes[i].thread_count = 0;
            g_processes[i].handle_count = 0;
            g_processes[i].exit_code = 0;
            g_processes[i].exited = 0;
            g_processes[i].create_time_ms = timer_get_ticks() * 10;
            g_processes[i].heap_usage = 0;
            memset(g_processes[i].threads, 0, sizeof(g_processes[i].threads));
            memset(g_processes[i].handles, 0, sizeof(g_processes[i].handles));
            if (pid_out) *pid_out = g_processes[i].pid;
            return 0;
        }
    }
    return -1;
}

compat_process_t *compat_process_get(uint64_t pid)
{
    for (int i = 0; i < COMPAT_MAX_PROCESSES; i++) {
        if (g_processes[i].used && g_processes[i].pid == pid) {
            return &g_processes[i];
        }
    }
    return NULL;
}

int compat_process_exit(uint64_t pid, uint32_t exit_code)
{
    compat_process_t *proc = compat_process_get(pid);
    if (!proc) return -1;
    proc->exit_code = exit_code;
    proc->exited = 1;
    kprintf("[compat-proc] process %llu exited with code %u\n", pid, exit_code);
    return 0;
}

int compat_process_add_thread(uint64_t pid, uint32_t tid)
{
    compat_process_t *proc = compat_process_get(pid);
    if (!proc) return -1;
    for (uint32_t i = 0; i < COMPAT_MAX_THREADS_PER_PROC; i++) {
        if (!proc->threads[i].used) {
            memset(&proc->threads[i], 0, sizeof(proc->threads[i]));
            proc->threads[i].used = 1;
            proc->threads[i].tid = tid;
            proc->threads[i].pid = pid;
            proc->thread_count++;
            return 0;
        }
    }
    return -1;
}

int compat_process_list(void)
{
    kprintf("[compat-proc] process list:\n");
    uint32_t count = 0;
    for (int i = 0; i < COMPAT_MAX_PROCESSES; i++) {
        if (g_processes[i].used) {
            kprintf("  pid=%llu name=\"%s\" threads=%u handles=%u exited=%d code=%u\n",
                    g_processes[i].pid, g_processes[i].name,
                    g_processes[i].thread_count, g_processes[i].handle_count,
                    g_processes[i].exited, g_processes[i].exit_code);
            count++;
        }
    }
    return (int)count;
}

uint32_t compat_process_count(void)
{
    uint32_t count = 0;
    for (int i = 0; i < COMPAT_MAX_PROCESSES; i++) {
        if (g_processes[i].used) count++;
    }
    return count;
}
