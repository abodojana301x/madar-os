#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <types.h>

typedef struct {
    volatile int locked;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);
int spinlock_try_lock(spinlock_t *lock);

/* Backward-compatible names used by some drivers */
static inline void spinlock_acquire(spinlock_t *lock) { spinlock_lock(lock); }
static inline void spinlock_release(spinlock_t *lock) { spinlock_unlock(lock); }

#endif
