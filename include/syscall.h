/*
 * Layer: 2 (Kernel Core) - Syscall Interface
 * File: syscall.h
 * Purpose: System call interface definitions
 * Status: New for Layer 2
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
#include <syscalls_list.h>

/* Syscall function type */
typedef uint64_t (*syscall_fn_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

/* External declarations */
extern syscall_fn_t syscall_table[];

typedef struct {
    uint64_t total_calls;
    uint64_t invalid_calls;
    uint64_t stub_calls;
} syscall_stats_t;

/* Function Prototypes */
void syscall_init(void);
uint64_t syscall_dispatcher(uint64_t syscall_num,
                           uint64_t arg1, uint64_t arg2, uint64_t arg3,
                           uint64_t arg4, uint64_t arg5, uint64_t arg6);
void syscall_get_stats(syscall_stats_t *stats);

/* Syscall implementations */
uint64_t sys_print(uint64_t str_ptr, uint64_t len, uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t sys_exit(uint64_t code, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t sys_get_tick(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

#endif /* SYSCALL_H */
