/*
 * Layer: 4 (System Services) - Init Process Implementation
 * File: init.c
 * Purpose: PID 1 logic, service startup
 */

#include "init.h"
#include "service_mgr.h"
#include <kprintf.h>
#include <process.h>
#include <scheduler.h>
#include <timer.h>

void init_start(void) {
    kprintf("🚀 Starting Init (PID 1)...\n");

    /* Initialize service manager */
    service_mgr_init();

    /* Parse config and start services */
    config_parse(INIT_CFG_PATH);

    kprintf("✅ Init started\n");
}

void init_main_loop(void) {
    kprintf("⏳ Init entering main loop...\n");

    uint64_t last_check = timer_get_ticks();

    while (1) {
        uint64_t now = timer_get_ticks();
        if ((now - last_check) >= INIT_CHECK_INTERVAL) {
            service_mgr_check_all();
            last_check = now;
        }

        /* Yield to allow other processes to run */
        thread_yield();
    }
}
