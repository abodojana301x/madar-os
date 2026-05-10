/*
 * Layer: 2 (Kernel Core) - Scheduler Implementation
 * File: scheduler.c
 * Purpose: Round-robin scheduler with time quantum
 * Status: New for Layer 2
 */

#include <scheduler.h>
#include <process.h>
#include <kprintf.h>
#include <panic.h>

/* External thread reference */
extern pcb_t *current_thread;

/* Scheduler state */
static scheduler_stats_t scheduler_stats = {0};

/* Forward declarations for context switching */
static void context_switch_to(pcb_t *thread)
{
    if (!thread) return;
    
    /* TODO: Implement actual register save/restore */
    /* For now this is a stub that just sets RSP */
    asm volatile("mov %0, %%rsp" : : "r"(thread->rsp) : "memory");
}

static void context_switch(pcb_t *old_thread, pcb_t *new_thread)
{
    if (!new_thread) return;
    (void)old_thread;
    
    /* TODO: Implement full register save/restore */
    /* For now, just switch stack pointers */
    context_switch_to(new_thread);
}

/*
 * Initialize scheduler
 */
void scheduler_init(void)
{
    kprintf("⏰ Initializing Round-Robin Scheduler...\n");
    kprintf("  Time quantum: %d ms\n", TIME_QUANTUM_MS);

    /* Create idle thread if needed */
    /* For now, assume at least one thread exists */

    kprintf("✅ Scheduler initialized\n");
}

/*
 * Main scheduling function
 */
void schedule(void)
{
    scheduler_stats.schedule_count++;

    if (!current_thread) {
        /* No current thread, get next ready thread */
        current_thread = pcb_get_next_ready();
        if (current_thread) {
            current_thread->state = THREAD_RUNNING;
            scheduler_stats.context_switch_count++;
            /* Context switch to new thread */
            context_switch_to(current_thread);
        } else {
            scheduler_stats.idle_returns++;
        }
        return;
    }

    if (current_thread->state == THREAD_RUNNING) {
        /* Save current thread state */
        current_thread->state = THREAD_READY;
        pcb_add_to_ready_queue(current_thread);
    }

    /* Get next thread */
    pcb_t *next_thread = pcb_get_next_ready();
    if (next_thread) {
        next_thread->state = THREAD_RUNNING;

        /* Context switch */
        context_switch(current_thread, next_thread);
        current_thread = next_thread;
        scheduler_stats.context_switch_count++;
    } else {
        /* No ready threads, keep current */
        if (current_thread && current_thread->state != THREAD_EXITED) {
            current_thread->state = THREAD_RUNNING;
        } else {
            current_thread = NULL;
            scheduler_stats.idle_returns++;
        }
    }
}

/*
 * Called on timer tick
 */
void scheduler_tick(void)
{
    scheduler_stats.tick_count++;

    /* Check if time quantum expired */
    if (scheduler_stats.tick_count % TIME_QUANTUM_MS == 0) {
        scheduler_stats.quantum_expirations++;
        schedule();
    }
}

/*
 * Add thread to scheduler
 */
void scheduler_add_thread(pcb_t *thread)
{
    if (thread) {
        scheduler_stats.add_requests++;
        pcb_add_to_ready_queue(thread);
    }
}

/*
 * Remove thread from scheduler
 */
void scheduler_remove_thread(pcb_t *thread)
{
    /* For now, just mark as exited */
    if (thread) {
        scheduler_stats.remove_requests++;
        thread->state = THREAD_EXITED;
    }
}

void scheduler_note_yield(void)
{
    scheduler_stats.yields++;
}

void scheduler_get_stats(scheduler_stats_t *stats)
{
    if (!stats) {
        return;
    }

    *stats = scheduler_stats;
}
