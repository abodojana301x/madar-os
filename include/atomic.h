/*
 * Layer: 2 (Kernel Core) - Atomic Operations
 * File: atomic.h
 * Purpose: Atomic operation interfaces
 * Status: New for Layer 2
 */

#ifndef ATOMIC_H
#define ATOMIC_H

#include <types.h>

/* Function Prototypes */
void atomic_inc(uint32_t *value);
void atomic_dec(uint32_t *value);
uint32_t atomic_cas(uint32_t *ptr, uint32_t old_val, uint32_t new_val);
uint32_t atomic_load(uint32_t *ptr);
void atomic_store(uint32_t *ptr, uint32_t value);

#endif /* ATOMIC_H */