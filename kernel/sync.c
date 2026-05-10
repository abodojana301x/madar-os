/*
 * Layer: 2 (Kernel Core) - Synchronization Implementation
 * File: sync.c
 * Purpose: Mutex implementation with thread yielding
 * Status: New for Layer 2
 */

#include <sync.h>
#include <process.h>
#include <thread.h>
#include <kprintf.h>

/* External thread reference */
extern pcb_t *current_thread;
static mutex_stats_t mutex_stats = {0};

/*
 * Initialize mutex
 */
void mutex_init(mutex_t *mutex)
{
    spinlock_init(&mutex->lock);
    mutex->owner = NULL;
    mutex->recursion_count = 0;
}

/*
 * Lock mutex
 */
void mutex_lock(mutex_t *mutex)
{
    mutex_stats.lock_calls++;
    /* Try to acquire spinlock */
    spinlock_lock(&mutex->lock);

    if (mutex->owner == current_thread) {
        /* Recursive lock */
        mutex->recursion_count++;
        spinlock_unlock(&mutex->lock);
        return;
    }

    /* Wait for ownership */
    while (mutex->owner != NULL) {
        mutex_stats.contention_loops++;
        spinlock_unlock(&mutex->lock);
        thread_yield();  /* Yield and try again */
        spinlock_lock(&mutex->lock);
    }

    /* Acquire ownership */
    mutex->owner = current_thread;
    mutex->recursion_count = 1;
    spinlock_unlock(&mutex->lock);
}

/*
 * Unlock mutex
 */
void mutex_unlock(mutex_t *mutex)
{
    mutex_stats.unlock_calls++;
    spinlock_lock(&mutex->lock);

    if (mutex->owner != current_thread) {
        mutex_stats.owner_violations++;
        spinlock_unlock(&mutex->lock);
        return;  /* Not the owner */
    }

    mutex->recursion_count--;
    if (mutex->recursion_count == 0) {
        mutex->owner = NULL;
    }

    spinlock_unlock(&mutex->lock);
}

/*
 * Try to lock mutex (non-blocking)
 */
int mutex_try_lock(mutex_t *mutex)
{
    mutex_stats.try_lock_calls++;
    if (!spinlock_try_lock(&mutex->lock)) {
        return 0;  /* Failed to get spinlock */
    }

    if (mutex->owner == current_thread) {
        /* Recursive lock */
        mutex->recursion_count++;
        spinlock_unlock(&mutex->lock);
        return 1;
    }

    if (mutex->owner != NULL) {
        spinlock_unlock(&mutex->lock);
        return 0;  /* Already owned */
    }

    /* Acquire ownership */
    mutex->owner = current_thread;
    mutex->recursion_count = 1;
    spinlock_unlock(&mutex->lock);
    return 1;
}

void mutex_get_stats(mutex_stats_t *stats)
{
    if (!stats) {
        return;
    }

    *stats = mutex_stats;
}
