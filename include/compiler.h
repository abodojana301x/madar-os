/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Compiler-specific macros and attributes
 * Constraints: GCC-specific
 */

#ifndef COMPILER_H
#define COMPILER_H

// Function attributes
#define UNUSED __attribute__((unused))
#define PACKED_ATTR __attribute__((packed))
#define ALWAYS_INLINE __attribute__((always_inline))
#define NORETURN __attribute__((noreturn))
#define ALIGNED(x) __attribute__((aligned(x)))

// Branch prediction hints
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// Memory barriers
#define COMPILER_BARRIER() __asm__ __volatile__("" ::: "memory")
#define MEMORY_BARRIER() __asm__ __volatile__("mfence" ::: "memory")

// Static assertions
#define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)

#endif