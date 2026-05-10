/*
 * Layer: 2 (Kernel Core) - Process Control Block Implementation
 * File: process.c
 * Purpose: PCB creation and management
 * Status: Simplified for Layer 2
 */

#include <process.h>
#include <stack.h>
#include <heap.h>
#include <kprintf.h>
#include <panic.h>
#include <string.h>
#include <memory_layout.h>

#define PCB_MAGIC 0x50434231u

/* Global thread management */
pcb_t *current_thread = NULL;
static uint32_t next_pid = 1;

/* Ready queue management */
static pcb_t *ready_queue_head = NULL;
static pcb_t *ready_queue_tail = NULL;
static uint32_t ready_queue_count = 0;
static process_stats_t process_stats = {0};

/*
 * Create a new PCB (internal helper)
 */
pcb_t *pcb_create(thread_entry_t entry, void *arg)
{
    /* Allocate PCB */
    pcb_t *pcb = (pcb_t *)kmalloc(sizeof(pcb_t));
    if (!pcb) {
        kprintf("Failed to allocate PCB\n");
        return NULL;
    }

    /* Initialize PCB */
    memset(pcb, 0, sizeof(pcb_t));
    pcb->pid = next_pid++;
    pcb->magic = PCB_MAGIC;
    pcb->state = THREAD_READY;
    pcb->arg = arg;
    pcb->next_ready = NULL;
    pcb->prev_ready = NULL;
    pcb->in_ready_queue = 0;

    /* Allocate kernel stack */
    pcb->stack = (uint64_t *)kernel_stack_alloc();
    if (!pcb->stack) {
        kfree(pcb);
        kprintf("Failed to allocate kernel stack\n");
        return NULL;
    }

    /* Set up initial stack frame */
    uint64_t *stack_top = pcb->stack + (KERNEL_STACK_SIZE / sizeof(uint64_t)) - 1;

    /* Simulate interrupt frame for context switch */
    *stack_top-- = 0x10;                    /* SS */
    stack_top--;
    *stack_top = (uint64_t)stack_top + 8; /* RSP */
    *stack_top-- = 0x202;                   /* RFLAGS (IF enabled) */
    *stack_top-- = 0x08;                    /* CS */
    *stack_top-- = (uint64_t)entry;         /* RIP */

    /* Push function argument */
    *stack_top-- = (uint64_t)arg;           /* Argument to function */

    /* Push dummy registers (will be restored by context switch) */
    *stack_top-- = 0; /* R15 */
    *stack_top-- = 0; /* R14 */
    *stack_top-- = 0; /* R13 */
    *stack_top-- = 0; /* R12 */
    *stack_top-- = 0; /* R11 */
    *stack_top-- = 0; /* R10 */
    *stack_top-- = 0; /* R9 */
    *stack_top-- = 0; /* R8 */
    *stack_top-- = 0; /* RBP */
    *stack_top-- = 0; /* RDI */
    *stack_top-- = 0; /* RSI */
    *stack_top-- = 0; /* RDX */
    *stack_top-- = 0; /* RCX */
    *stack_top-- = 0; /* RBX */
    *stack_top = 0;   /* RAX */

    /* Set RSP to point to the stack frame */
    pcb->rsp = (uint64_t)stack_top;
    process_stats.total_created++;
    process_stats.live_threads++;

    return pcb;
}

/*
 * Create a new thread (public API wrapper)
 */
pcb_t *thread_create(thread_entry_t entry, void *arg)
{
    pcb_t *pcb = pcb_create(entry, arg);
    if (pcb) {
        kprintf("Created thread %u\n", pcb->pid);
    }
    return pcb;
}

/*
 * Add thread to ready queue
 */
void pcb_add_to_ready_queue(pcb_t *pcb)
{
    if (!pcb) return;
    KASSERT(pcb->magic == PCB_MAGIC, "pcb_add_to_ready_queue on invalid pcb");
    if (pcb->state == THREAD_EXITED || pcb->in_ready_queue) {
        return;
    }
    
    pcb->state = THREAD_READY;
    pcb->next_ready = NULL;
    
    if (ready_queue_tail == NULL) {
        ready_queue_head = pcb;
        ready_queue_tail = pcb;
        pcb->prev_ready = NULL;
    } else {
        pcb->prev_ready = ready_queue_tail;
        ready_queue_tail->next_ready = pcb;
        ready_queue_tail = pcb;
    }
    
    pcb->in_ready_queue = 1;
    ready_queue_count++;
}

/*
 * Remove thread from ready queue
 */
static void pcb_remove_from_ready_queue(pcb_t *pcb)
{
    if (!pcb) return;
    KASSERT(pcb->magic == PCB_MAGIC, "pcb_remove_from_ready_queue on invalid pcb");
    if (!pcb->in_ready_queue) {
        return;
    }
    
    if (pcb->prev_ready)
        pcb->prev_ready->next_ready = pcb->next_ready;
    else
        ready_queue_head = pcb->next_ready;
    
    if (pcb->next_ready)
        pcb->next_ready->prev_ready = pcb->prev_ready;
    else
        ready_queue_tail = pcb->prev_ready;
    
    pcb->next_ready = NULL;
    pcb->prev_ready = NULL;
    pcb->in_ready_queue = 0;
    KASSERT(ready_queue_count > 0, "ready_queue_count underflow");
    ready_queue_count--;
}

/*
 * Destroy thread
 */
void pcb_destroy(pcb_t *pcb)
{
    if (!pcb) return;
    KASSERT(pcb->magic == PCB_MAGIC, "pcb_destroy on invalid pcb");
    
    /* Remove from queue if still there */
    if (pcb->in_ready_queue) {
        pcb_remove_from_ready_queue(pcb);
    }
    
    /* Free stack */
    if (pcb->stack) {
        kernel_stack_free(pcb->stack);
    }
    
    /* Free PCB */
    pcb->magic = 0;
    if (process_stats.live_threads > 0) {
        process_stats.live_threads--;
    }
    process_stats.total_destroyed++;
    kfree(pcb);
}

/*
 * Exit current thread
 */
extern void schedule(void);
void thread_exit(void)
{
    if (current_thread) {
        pcb_t *exiting = current_thread;
        KASSERT(exiting->magic == PCB_MAGIC, "thread_exit on invalid current thread");
        current_thread->state = THREAD_EXITED;
        process_stats.exited_threads++;
        current_thread = NULL;
        pcb_destroy(exiting);
        schedule();
    }
}

/*
 * Initialize process manager
 */
void process_init(void)
{
    ready_queue_head = NULL;
    ready_queue_tail = NULL;
    ready_queue_count = 0;
    current_thread = NULL;
    memset(&process_stats, 0, sizeof(process_stats));
    kprintf("Process manager initialized\n");
}

/*
 * Set current thread
 */
void scheduler_set_current(pcb_t *pcb)
{
    current_thread = pcb;
}

/*
 * Get next thread from ready queue
 */
pcb_t *pcb_get_next_ready(void)
{
    if (ready_queue_head) {
        pcb_t *pcb = ready_queue_head;
        pcb_remove_from_ready_queue(pcb);
        return pcb;
    }
    return NULL;
}

uint32_t pcb_get_ready_queue_count(void)
{
    return ready_queue_count;
}

void process_get_stats(process_stats_t *stats)
{
    if (!stats) {
        return;
    }

    *stats = process_stats;
    stats->ready_threads = ready_queue_count;
    stats->running_threads = (current_thread && current_thread->state == THREAD_RUNNING) ? 1u : 0u;
    stats->blocked_threads = (current_thread && current_thread->state == THREAD_BLOCKED) ? 1u : 0u;
}
