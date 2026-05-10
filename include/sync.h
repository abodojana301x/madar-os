/*
 * Layer: 2 (Kernel Core) - Synchronization Primitives
 * File: sync.h
 * Purpose: Mutex and semaphore interfaces
 * Status: New for Layer 2
 */

#ifndef SYNC_H
#define SYNC_H

#include <types.h>
#include <spinlock.h>
#include <process.h>

/* Mutex structure */
typedef struct {
    spinlock_t lock;
    pcb_t *owner;
    uint32_t recursion_count;
} mutex_t;

typedef struct {
    uint64_t lock_calls;
    uint64_t try_lock_calls;
    uint64_t unlock_calls;
    uint64_t contention_loops;
    uint64_t owner_violations;
} mutex_stats_t;

/* Function Prototypes */
void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);
int mutex_try_lock(mutex_t *mutex);
void mutex_get_stats(mutex_stats_t *stats);

#endif /* SYNC_H */
