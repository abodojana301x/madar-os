#include "sync_mapper.h"
#include "pe_threads.h"
#include <string.h>
#include <kprintf.h>

#define COMPAT_MAX_SYNC_OBJECTS 32

static compat_sync_object_t g_sync_objs[COMPAT_MAX_SYNC_OBJECTS];
static uint32_t g_next_sync_id = 0x6000;
static int g_sync_init = 0;

int compat_sync_init(void)
{
    if (g_sync_init) return 0;
    memset(g_sync_objs, 0, sizeof(g_sync_objs));
    g_sync_init = 1;
    kprintf("[compat-sync] sync subsystem initialized\n");
    return 0;
}

uint32_t compat_sync_create(int type, int initial_signaled, int manual_reset)
{
    if (!g_sync_init) compat_sync_init();
    for (int i = 0; i < COMPAT_MAX_SYNC_OBJECTS; i++) {
        if (!g_sync_objs[i].used) {
            g_sync_objs[i].used = 1;
            g_sync_objs[i].id = g_next_sync_id++;
            g_sync_objs[i].type = type;
            g_sync_objs[i].signaled = initial_signaled ? 1 : 0;
            g_sync_objs[i].manual_reset = manual_reset ? 1 : 0;
            g_sync_objs[i].waiters = 0;
            return g_sync_objs[i].id;
        }
    }
    return (uint32_t)-1;
}

int compat_sync_signal(uint32_t id)
{
    for (int i = 0; i < COMPAT_MAX_SYNC_OBJECTS; i++) {
        if (g_sync_objs[i].used && g_sync_objs[i].id == id) {
            g_sync_objs[i].signaled = 1;
            return 0;
        }
    }
    return -1;
}

int compat_sync_reset(uint32_t id)
{
    for (int i = 0; i < COMPAT_MAX_SYNC_OBJECTS; i++) {
        if (g_sync_objs[i].used && g_sync_objs[i].id == id) {
            g_sync_objs[i].signaled = 0;
            return 0;
        }
    }
    return -1;
}

int compat_wait_for_object(uint32_t object_id, uint32_t timeout_ms)
{
    (void)timeout_ms;
    for (int i = 0; i < COMPAT_MAX_SYNC_OBJECTS; i++) {
        if (g_sync_objs[i].used && g_sync_objs[i].id == object_id) {
            if (g_sync_objs[i].signaled) {
                if (!g_sync_objs[i].manual_reset) g_sync_objs[i].signaled = 0;
                return 0;
            }
            return -1;
        }
    }
    return compat_wait_thread(object_id, timeout_ms);
}

int compat_wait_for_multiple(uint32_t count, uint32_t *ids, int wait_all, uint32_t timeout_ms)
{
    (void)count; (void)ids; (void)wait_all; (void)timeout_ms;
    return 0;
}

void compat_sync_print(void)
{
    kprintf("[compat-sync] sync objects:\n");
    for (int i = 0; i < COMPAT_MAX_SYNC_OBJECTS; i++) {
        if (g_sync_objs[i].used) {
            kprintf("  [%d] id=%u type=%d signaled=%d manual=%d waiters=%u\n",
                    i, g_sync_objs[i].id, g_sync_objs[i].type,
                    g_sync_objs[i].signaled, g_sync_objs[i].manual_reset,
                    g_sync_objs[i].waiters);
        }
    }
}
