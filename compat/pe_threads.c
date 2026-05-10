#include "pe_threads.h"
#include <string.h>
#include <kprintf.h>
#include <timer.h>

static compat_thread_entry_t g_threads[COMPAT_MAX_THREADS];
static uint32_t g_next_tid = 1;

void compat_threads_init(void)
{
    memset(g_threads, 0, sizeof(g_threads));
    g_next_tid = 1;
}

static compat_thread_entry_t *find_thread(uint32_t tid)
{
    for (int i = 0; i < COMPAT_MAX_THREADS; i++) {
        if (g_threads[i].used && g_threads[i].tid == tid) {
            return &g_threads[i];
        }
    }
    return NULL;
}

int compat_create_thread(void (*entry)(void *), void *arg, uint32_t *tid_out)
{
    return compat_create_thread_pid(entry, arg, 0, tid_out);
}

int compat_create_thread_pid(void (*entry)(void *), void *arg, uint64_t pid, uint32_t *tid_out)
{
    for (int i = 0; i < COMPAT_MAX_THREADS; i++) {
        if (!g_threads[i].used) {
            g_threads[i].used = 1;
            g_threads[i].tid = g_next_tid++;
            g_threads[i].pid = pid;
            g_threads[i].state = COMPAT_THREAD_RUNNING;
            g_threads[i].entry = entry;
            g_threads[i].arg = arg;
            g_threads[i].start_time_ms = timer_get_ticks() * 10;
            memset(g_threads[i].tls_values, 0, sizeof(g_threads[i].tls_values));
            if (tid_out) *tid_out = g_threads[i].tid;
            if (entry) {
                entry(arg);
                g_threads[i].state = COMPAT_THREAD_DONE;
            }
            return 0;
        }
    }
    kprintf("[compat-threads] warning: thread table full\n");
    return -1;
}

int compat_set_tls(uint32_t tid, uint32_t slot, uint64_t value)
{
    compat_thread_entry_t *t = find_thread(tid);
    if (!t) return -1;
    if (slot >= COMPAT_TLS_MAX_SLOTS) return -1;
    t->tls_values[slot] = value;
    return 0;
}

int compat_get_tls(uint32_t tid, uint32_t slot, uint64_t *value_out)
{
    if (!value_out) return -1;
    compat_thread_entry_t *t = find_thread(tid);
    if (!t) return -1;
    if (slot >= COMPAT_TLS_MAX_SLOTS) return -1;
    *value_out = t->tls_values[slot];
    return 0;
}

int compat_wait_thread(uint32_t tid, uint32_t timeout_ms)
{
    (void)timeout_ms;
    compat_thread_entry_t *t = find_thread(tid);
    if (!t) return -1;
    return (t->state == COMPAT_THREAD_DONE) ? 0 : -1;
}

int compat_thread_state(uint32_t tid, compat_thread_state_t *state_out)
{
    compat_thread_entry_t *t = find_thread(tid);
    if (!t) return -1;
    if (state_out) *state_out = t->state;
    return 0;
}

uint32_t compat_thread_count(void)
{
    uint32_t count = 0;
    for (int i = 0; i < COMPAT_MAX_THREADS; i++) {
        if (g_threads[i].used && g_threads[i].state != COMPAT_THREAD_DONE) count++;
    }
    return count;
}

void compat_thread_cleanup(uint32_t tid)
{
    compat_thread_entry_t *t = find_thread(tid);
    if (t) {
        t->used = 0;
        t->state = COMPAT_THREAD_IDLE;
    }
}

void compat_schedule_threads(void)
{
    for (int i = 0; i < COMPAT_MAX_THREADS; i++) {
        if (g_threads[i].used && g_threads[i].state == COMPAT_THREAD_RUNNING && g_threads[i].entry) {
            g_threads[i].state = COMPAT_THREAD_RUNNING;
        }
    }
}
