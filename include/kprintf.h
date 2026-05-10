/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: واجهة طباعة موحدة للشاشة والـ Serial
 * Constraints: لا تستخدم libc
 */

#ifndef KPRINTF_H
#define KPRINTF_H

#include <types.h>
#include <compiler.h>

// Initialize output systems
void kprintf_init(void);

// Print formatted string (limited printf)
int kprintf(const char *format, ...);

// Print emergency message (before full init)
void kprintf_early(const char *str);

// Test function
void test_kprintf(void);

#endif