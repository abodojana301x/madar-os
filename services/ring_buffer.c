/*
 * Layer: 4 (System Services) - Ring Buffer Implementation
 * File: ring_buffer.c
 * Purpose: Thread-safe circular buffer for IPC
 */

#include "ring_buffer.h"
#include <string.h>

void ring_buffer_init(ring_buffer_t *rb) {
    if (!rb) return;
    memset(rb->data, 0, RING_BUFFER_SIZE);
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    spinlock_init(&rb->lock);
}

int ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint32_t len) {
    if (!rb || !data || len == 0) return -1;

    spinlock_lock(&rb->lock);

    uint32_t available = RING_BUFFER_SIZE - rb->count;
    if (len > available) {
        spinlock_unlock(&rb->lock);
        return -1; /* Not enough space */
    }

    for (uint32_t i = 0; i < len; i++) {
        rb->data[rb->head] = data[i];
        rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    }
    rb->count += len;

    spinlock_unlock(&rb->lock);
    return (int)len;
}

int ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint32_t len) {
    if (!rb || !data || len == 0) return -1;

    spinlock_lock(&rb->lock);

    if (len > rb->count) {
        len = rb->count; /* Read what's available */
    }

    for (uint32_t i = 0; i < len; i++) {
        data[i] = rb->data[rb->tail];
        rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    }
    rb->count -= len;

    spinlock_unlock(&rb->lock);
    return (int)len;
}

uint32_t ring_buffer_count(ring_buffer_t *rb) {
    if (!rb) return 0;
    uint32_t count;
    spinlock_lock(&rb->lock);
    count = rb->count;
    spinlock_unlock(&rb->lock);
    return count;
}

int ring_buffer_is_empty(ring_buffer_t *rb) {
    return ring_buffer_count(rb) == 0;
}

int ring_buffer_is_full(ring_buffer_t *rb) {
    return ring_buffer_count(rb) >= RING_BUFFER_SIZE;
}

