#ifndef COMPAT_PE_THREADS_H
#define COMPAT_PE_THREADS_H

#include <types.h>

#define COMPAT_MAX_THREADS 32
#define COMPAT_TLS_MAX_SLOTS 64

typedef enum {
    COMPAT_THREAD_IDLE,
    COMPAT_THREAD_RUNNING,
    COMPAT_THREAD_DONE,
    COMPAT_THREAD_ERROR
} compat_thread_state_t;

typedef struct {
    int used;
    uint32_t tid;
    uint64_t pid;
    compat_thread_state_t state;
    void (*entry)(void *);
    void *arg;
    uint64_t tls_values[COMPAT_TLS_MAX_SLOTS];
    uint64_t stack_base;
    uint32_t stack_size;
    uint64_t start_time_ms;
} compat_thread_entry_t;

void compat_threads_init(void);
int compat_create_thread(void (*entry)(void *), void *arg, uint32_t *tid_out);
int compat_create_thread_pid(void (*entry)(void *), void *arg, uint64_t pid, uint32_t *tid_out);
int compat_set_tls(uint32_t tid, uint32_t slot, uint64_t value);
int compat_get_tls(uint32_t tid, uint32_t slot, uint64_t *value_out);
int compat_wait_thread(uint32_t tid, uint32_t timeout_ms);
int compat_thread_state(uint32_t tid, compat_thread_state_t *state_out);
uint32_t compat_thread_count(void);
void compat_thread_cleanup(uint32_t tid);
void compat_schedule_threads(void);

#endif
