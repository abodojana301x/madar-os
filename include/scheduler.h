/*
 * Layer: 2 (Kernel Core) - Scheduler
 * File: scheduler.h
 * Purpose: Round-robin scheduler interface
 * Status: New for Layer 2
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>
#include <process.h>

/* Scheduler Configuration */
#define TIME_QUANTUM_MS     10      /* 10ms time quantum */

typedef struct {
    uint64_t tick_count;
    uint64_t schedule_count;
    uint64_t context_switch_count;
    uint64_t idle_returns;
    uint64_t quantum_expirations;
    uint64_t yields;
    uint64_t add_requests;
    uint64_t remove_requests;
} scheduler_stats_t;

/* Function Prototypes */
void scheduler_init(void);
void schedule(void);
void scheduler_tick(void);
void scheduler_add_thread(pcb_t *thread);
void scheduler_remove_thread(pcb_t *thread);
void scheduler_note_yield(void);
void scheduler_get_stats(scheduler_stats_t *stats);

#endif /* SCHEDULER_H */
