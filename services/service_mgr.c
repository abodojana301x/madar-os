/*
 * Layer: 4 (System Services) - Service Manager Implementation
 * File: service_mgr.c
 * Purpose: Monitor and auto-restart services
 */

#include "service_mgr.h"
#include <kprintf.h>
#include <string.h>
#include <timer.h>

static service_t services[MAX_SERVICES];
static int service_count = 0;

void service_mgr_init(void) {
    kprintf("🔧 Initializing Service Manager...\n");
    memset(services, 0, sizeof(services));
    service_count = 0;
    kprintf("✅ Service Manager initialized\n");
}

int service_mgr_register(const char *name) {
    if (!name || service_count >= MAX_SERVICES) return -1;

    service_t *svc = &services[service_count];
    strncpy(svc->name, name, SVC_NAME_LEN - 1);
    svc->name[SVC_NAME_LEN - 1] = '\0';
    svc->state = SVC_STOPPED;
    svc->pid = 0;
    svc->crash_count = 0;
    svc->backoff_delay = 2000; /* 2 seconds */
    svc->last_start = 0;
    svc->enabled = 1;

    kprintf("  Service registered: %s\n", svc->name);
    service_count++;
    return 0;
}

int service_mgr_start(const char *name) {
    if (!name) return -1;

    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0) {
            services[i].state = SVC_RUNNING;
            services[i].pid = i + 100; /* Fake PID */
            services[i].last_start = timer_get_ticks();
            services[i].crash_count = 0;
            kprintf("  Service started: %s (PID %u)\n", name, services[i].pid);
            return 0;
        }
    }
    return -1;
}

void service_mgr_stop(const char *name) {
    if (!name) return;

    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0) {
            services[i].state = SVC_STOPPED;
            services[i].pid = 0;
            kprintf("  Service stopped: %s\n", name);
            return;
        }
    }
}

static void service_restart(service_t *svc) {
    if (!svc || svc->crash_count >= SVC_MAX_CRASHES) {
        kprintf("  Service %s exceeded max crashes, disabling\n", svc->name);
        svc->enabled = 0;
        svc->state = SVC_STOPPED;
        return;
    }

    svc->state = SVC_RESTARTING;
    svc->crash_count++;
    svc->backoff_delay *= 2; /* Exponential backoff */

    kprintf("  Restarting %s in %u ms (crash #%u)\n",
            svc->name, svc->backoff_delay, svc->crash_count);

    /* Simulate restart */
    svc->state = SVC_RUNNING;
    svc->pid = svc->pid + 1;
    svc->last_start = timer_get_ticks();
}

void service_mgr_check_all(void) {
    uint64_t now = timer_get_ticks();

    for (int i = 0; i < service_count; i++) {
        service_t *svc = &services[i];
        if (!svc->enabled) continue;

        if (svc->state == SVC_CRASHED) {
            service_restart(svc);
        } else if (svc->state == SVC_RUNNING) {
            /* Simulate health check - random crash for testing */
            /* In real implementation, check if process exists */
            (void)now;
        }
    }
}

void service_mgr_simulate_crash(const char *name) {
    if (!name) return;

    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0) {
            services[i].state = SVC_CRASHED;
            services[i].pid = 0;
            kprintf("💥 Service crashed: %s\n", name);
            return;
        }
    }
}

