/*
 * Layer: 4 (System Services) - Service Manager
 * File: service_mgr.h
 * Purpose: Monitor and auto-restart services
 */

#ifndef SERVICE_MGR_H
#define SERVICE_MGR_H

#include <types.h>

#define MAX_SERVICES        16
#define SVC_NAME_LEN        32
#define SVC_MAX_CRASHES     5

typedef enum {
    SVC_STOPPED,
    SVC_RUNNING,
    SVC_CRASHED,
    SVC_RESTARTING
} svc_state_t;

typedef struct {
    char name[SVC_NAME_LEN];
    svc_state_t state;
    uint32_t pid;
    uint32_t crash_count;
    uint32_t backoff_delay;  /* ms */
    uint64_t last_start;
    int enabled;
} service_t;

void service_mgr_init(void);
int service_mgr_register(const char *name);
int service_mgr_start(const char *name);
void service_mgr_check_all(void);
void service_mgr_stop(const char *name);

/* Config parser */
int config_parse(const char *path);

#endif

