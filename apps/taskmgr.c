/*
 * Layer: 7 (User Applications) - Task Manager and Launcher
 * File: taskmgr.c
 * Purpose: Lightweight process/resource monitor with safe launcher hooks.
 */

#include "taskmgr.h"
#include "kprintf.h"
#include "string.h"

int process_list_snapshot(char *buf, unsigned long size);
int resource_monitor_sample(unsigned *cpu, unsigned *ram);
int kill_handler_request(unsigned pid);

int taskmgr_init(void) {
    kprintf("Task Manager initialized\n");
    return 0;
}

int taskmgr_selftest(void) {
    char list[256];
    unsigned cpu = 0, ram = 0;
    kprintf("Testing Task Manager...\n");
    if (process_list_snapshot(list, sizeof(list)) <= 0) return -1;
    if (!strstr(list, "init")) return -1;
    if (resource_monitor_sample(&cpu, &ram) != 0) return -1;
    if (kill_handler_request(9999) != 0) return -1;
    if (launcher_start("shell") != 0) return -1;
    kprintf("Task Manager PASSED\n");
    return 0;
}
