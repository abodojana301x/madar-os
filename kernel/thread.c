/*
 * Layer: 2 (Kernel Core) - Thread Management Implementation
 * File: thread.c
 * Purpose: Additional thread utilities
 * Status: Integrated with process.c
 */

#include <thread.h>
#include <process.h>
#include <kprintf.h>
#include <scheduler.h>

/*
 * Yield CPU to next thread
 */
void thread_yield(void)
{
    /* Request scheduler to context switch */
    extern void schedule(void);
    scheduler_note_yield();
    schedule();
}
