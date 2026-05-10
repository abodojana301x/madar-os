/*
 * Layer: 4 (System Services) - Signal Handling
 * File: signal.h
 * Purpose: Process signal delivery and handling
 */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <types.h>
#include <process.h>

#define MAX_SIGNALS         32
#define SIGTERM             15
#define SIGKILL             9
#define SIGINT              2
#define SIGUSR1             10
#define SIGUSR2             12
#define SIGCHLD             17

#define SIG_DFL             ((void (*)(int))0)
#define SIG_IGN             ((void (*)(int))1)

typedef void (*sighandler_t)(int);

/* Signal context per process */
typedef struct {
    uint32_t pending_mask;      /* Bitmask of pending signals */
    uint32_t blocked_mask;      /* Bitmask of blocked signals */
    sighandler_t handlers[MAX_SIGNALS];
} signal_state_t;

/* Signal API */
void signal_init(void);
int signal_register_handler(uint32_t sig, sighandler_t handler);
int signal_send(uint32_t pid, uint32_t sig);
int signal_deliver(pcb_t *proc);
void signal_default_handler(int sig);

#endif
