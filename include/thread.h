/*
 * Layer: 2 (Kernel Core) - Thread Management
 * File: thread.h
 * Purpose: Thread creation and management interface
 * Status: New for Layer 2
 */

#ifndef THREAD_H
#define THREAD_H

#include <types.h>
#include <process.h>  /* For pcb_t */

/* Thread entry function type */
typedef void (*thread_entry_t)(void*);

/* Function Prototypes */
pcb_t *thread_create(thread_entry_t entry, void *arg);
void thread_exit(void);
void thread_yield(void);

#endif /* THREAD_H */