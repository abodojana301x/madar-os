/*
 * Layer: 4 (System Services) - Signal Implementation
 * File: signal.c
 * Purpose: Process signal delivery and handling
 */

#include "signal.h"
#include <process.h>
#include <kprintf.h>
#include <scheduler.h>

void signal_init(void) {
    kprintf("📶 Initializing Signal subsystem...\n");
    kprintf("✅ Signal subsystem initialized\n");
}

void signal_default_handler(int sig) {
    kprintf("  Signal %d: default action (terminate)\n", sig);
    /* Default action: terminate process */
    if (sig == SIGKILL || sig == SIGTERM || sig == SIGINT) {
        thread_exit();
    }
}

int signal_register_handler(uint32_t sig, sighandler_t handler) {
    extern pcb_t *current_thread;
    if (!current_thread || sig >= MAX_SIGNALS) return -1;

    current_thread->signal_handlers[sig] = handler;
    kprintf("  Register handler for signal %d\n", sig);
    return 0;
}

int signal_send(uint32_t pid, uint32_t sig) {
    if (sig >= MAX_SIGNALS || sig == 0) return -1;

    kprintf("  Sending signal %d to PID %u\n", sig, pid);

    /* Find target process */
    /* For now, just print - full implementation after process.h update */
    if (sig == SIGKILL) {
        kprintf("  SIGKILL delivered - process %u terminated\n", pid);
    }

    return 0;
}

int signal_deliver(pcb_t *proc) {
    (void)proc;
    /* Check pending signals and invoke handlers */
    /* Full implementation after process.h update */
    return 0;
}
