#ifndef COMPAT_SYNC_MAPPER_H
#define COMPAT_SYNC_MAPPER_H

#include <types.h>

#define COMPAT_SYNC_EVENT    1
#define COMPAT_SYNC_MUTEX    2
#define COMPAT_SYNC_SEMAPHORE 3
#define COMPAT_SYNC_THREAD    4

typedef struct {
    int used;
    uint32_t id;
    int type;
    int signaled;
    int manual_reset;
    uint32_t waiters;
} compat_sync_object_t;

int compat_sync_init(void);
uint32_t compat_sync_create(int type, int initial_signaled, int manual_reset);
int compat_sync_signal(uint32_t id);
int compat_sync_reset(uint32_t id);
int compat_wait_for_object(uint32_t object_id, uint32_t timeout_ms);
int compat_wait_for_multiple(uint32_t count, uint32_t *ids, int wait_all, uint32_t timeout_ms);
void compat_sync_print(void);

#endif
