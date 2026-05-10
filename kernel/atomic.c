/*
 * Layer: 2 (Kernel Core) - Atomic Operations Implementation
 * File: atomic.c
 * Purpose: GCC atomic built-ins wrapper
 * Status: New for Layer 2
 */

#include <atomic.h>

/*
 * Atomic increment
 */
void atomic_inc(uint32_t *value)
{
    __atomic_add_fetch(value, 1, __ATOMIC_SEQ_CST);
}

/*
 * Atomic decrement
 */
void atomic_dec(uint32_t *value)
{
    __atomic_sub_fetch(value, 1, __ATOMIC_SEQ_CST);
}

/*
 * Atomic compare and swap
 */
uint32_t atomic_cas(uint32_t *ptr, uint32_t old_val, uint32_t new_val)
{
    uint32_t expected = old_val;
    __atomic_compare_exchange_n(ptr, &expected, new_val, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
}

/*
 * Atomic load
 */
uint32_t atomic_load(uint32_t *ptr)
{
    return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

/*
 * Atomic store
 */
void atomic_store(uint32_t *ptr, uint32_t value)
{
    __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}