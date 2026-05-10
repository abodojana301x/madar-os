/*
 * Layer: 2 (Kernel Core) - Spinlock Implementation
 * File: spinlock.c
 * Purpose: Basic spinlock with pause instruction
 * Status: New for Layer 2
 */

#include <spinlock.h>

/*
 * Initialize spinlock
 */
void spinlock_init(spinlock_t *lock)
{
    lock->locked = 0;
}

/*
 * Acquire spinlock
 */
void spinlock_lock(spinlock_t *lock)
{
    while (1) {
        /* Try to acquire lock */
        if (__atomic_exchange_n(&lock->locked, 1, __ATOMIC_ACQUIRE) == 0) {
            return;
        }

        /* Wait with pause to reduce CPU usage */
        while (lock->locked) {
            __asm__ volatile("pause");
        }
    }
}

/*
 * Release spinlock
 */
void spinlock_unlock(spinlock_t *lock)
{
    __atomic_store_n(&lock->locked, 0, __ATOMIC_RELEASE);
}

/*
 * Try to acquire spinlock (non-blocking)
 */
int spinlock_try_lock(spinlock_t *lock)
{
    return __atomic_exchange_n(&lock->locked, 1, __ATOMIC_ACQUIRE) == 0;
}