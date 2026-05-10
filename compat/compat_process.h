#ifndef COMPAT_PROCESS_H
#define COMPAT_PROCESS_H

#include <types.h>
#include "pe_loader.h"

#define COMPAT_MAX_PROCESSES 8
#define COMPAT_MAX_THREADS_PER_PROC 16
#define COMPAT_MAX_HANDLES_PER_PROC 64
#define COMPAT_MAX_MODULES_PER_PROC 16
#define COMPAT_PROCESS_NAME_LEN 64

typedef struct {
    int used;
    uint32_t tid;
    uint64_t tls_value;
    void (*entry)(void *);
    void *arg;
    uint64_t pid;
    int running;
} compat_process_thread_t;

typedef struct {
    uint64_t handle_id;
    int used;
    int type;
    void *data;
} compat_process_handle_t;

typedef struct {
    int used;
    char name[COMPAT_PROCESS_NAME_LEN];
    uint64_t pid;
    uint8_t *image_base;
    uint64_t entry_point;
    uint32_t mapped_size;
    compat_process_thread_t threads[COMPAT_MAX_THREADS_PER_PROC];
    uint32_t thread_count;
    compat_process_handle_t handles[COMPAT_MAX_HANDLES_PER_PROC];
    uint32_t handle_count;
    uint32_t exit_code;
    int exited;
    uint64_t create_time_ms;
    uint64_t heap_usage;
} compat_process_t;

void compat_process_init(void);
int compat_process_create(const char *name, uint8_t *image_base, uint64_t entry_point, uint32_t mapped_size, uint64_t *pid_out);
compat_process_t *compat_process_get(uint64_t pid);
int compat_process_exit(uint64_t pid, uint32_t exit_code);
int compat_process_add_thread(uint64_t pid, uint32_t tid);
int compat_process_list(void);
uint32_t compat_process_count(void);

#endif
