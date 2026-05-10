/*
 * Layer: 2 PCB definitions
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <types.h>
#include <memory_layout.h>

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_EXITED
} thread_state_t;

typedef void (*thread_entry_t)(void*);

typedef struct pcb {
    uint32_t pid;
    uint32_t magic;
    thread_state_t state;
    uint64_t *stack;
    uint64_t stack_top;
    uint64_t rsp;
    uint64_t rip;
    void *arg;
    
    /* Registers context */
    uint64_t r15, r14, r13, r12, rbx, rbp;
    uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;
    uint64_t rflags;
    
    /* Layer 4: Security */
    uint32_t uid;
    uint32_t gid;
    
    /* Layer 4: Signals */
    uint32_t pending_signals;
    uint32_t blocked_signals;
    void (*signal_handlers[32])(int);
    
    /* List pointers */
    struct pcb *next_ready;
    struct pcb *prev_ready;
    uint8_t in_ready_queue;
} pcb_t;

typedef struct {
    uint32_t live_threads;
    uint32_t ready_threads;
    uint32_t running_threads;
    uint32_t blocked_threads;
    uint32_t exited_threads;
    uint32_t total_created;
    uint32_t total_destroyed;
} process_stats_t;

/* Thread functions */
pcb_t *thread_create(thread_entry_t entry, void *arg);
void thread_exit(void);
void thread_yield(void);

/* PCB functions */
pcb_t *pcb_create(thread_entry_t entry, void *arg);
void pcb_add_to_ready_queue(pcb_t *pcb);
void pcb_destroy(pcb_t *pcb);
pcb_t *pcb_get_next_ready(void);
uint32_t pcb_get_ready_queue_count(void);
void process_get_stats(process_stats_t *stats);

/* Process manager */
void process_init(void);
void scheduler_set_current(pcb_t *pcb);

#endif
