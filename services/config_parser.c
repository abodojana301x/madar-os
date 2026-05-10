/*
 * Layer: 4 (System Services) - Config Parser
 * File: config_parser.c
 * Purpose: Parse /etc/init.cfg (key=value format)
 */

#include "service_mgr.h"
#include <kprintf.h>
#include <string.h>

/* Built-in default config (since we may not have filesystem yet) */
static const char *default_config =
    "# Madar OS init configuration\n"
    "service=vfs\n"
    "service=fat32\n"
    "service=ipc\n"
    "service=net\n"
    "service=security\n";

int config_parse(const char *path) {
    (void)path;
    kprintf("📖 Parsing init config...\n");

    /* For now, use built-in defaults */
    const char *p = default_config;
    char line[64];
    int line_idx = 0;

    while (*p) {
        if (*p == '\n' || line_idx >= 63) {
            line[line_idx] = '\0';

            /* Parse line */
            if (strncmp(line, "service=", 8) == 0) {
                char svc_name[SVC_NAME_LEN];
                strncpy(svc_name, line + 8, SVC_NAME_LEN - 1);
                svc_name[SVC_NAME_LEN - 1] = '\0';

                service_mgr_register(svc_name);
                service_mgr_start(svc_name);
            }

            line_idx = 0;
        } else {
            line[line_idx++] = *p;
        }
        p++;
    }

    kprintf("✅ Config parsed\n");
    return 0;
}

