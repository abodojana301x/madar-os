/*
 * Layer: 2 (Kernel Core) - Syscall Interface
 * File: syscalls_list.h
 * Purpose: System call numbers and definitions
 * Status: New for Layer 2
 */

#ifndef SYSCALLS_LIST_H
#define SYSCALLS_LIST_H

/* System call numbers */
#define SYS_PRINT        0
#define SYS_EXIT         1
#define SYS_GET_TICK     2
#define SYS_OPEN         3
#define SYS_READ         4
#define SYS_WRITE        5
#define SYS_CLOSE        6
#define SYS_MOUNT        7
#define SYS_PIPE         8
#define SYS_KILL         9
#define SYS_SETUID       10
#define SYS_CHMOD        11
#define SYS_EXEC_APP     12
#define SYS_GET_CONFIG   13
#define SYS_HTTP_REQUEST 14
#define SYS_LIST_PROCS   15
#define MAX_SYSCALLS     16

#endif /* SYSCALLS_LIST_H */