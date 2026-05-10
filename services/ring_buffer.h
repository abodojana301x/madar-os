/*
 * Layer: 4 (System Services) - Ring Buffer
 * File: ring_buffer.h
 * Purpose: Thread-safe circular buffer for IPC
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <types.h>
#include <spinlock.h>

#define RING_BUFFER_SIZE 4096

typedef struct {
    uint8_t data[RING_BUFFER_SIZE];
    uint32_t head;      /* Write position */
    uint32_t tail;      /* Read position */
    uint32_t count;     /* Bytes currently in buffer */
    spinlock_t lock;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb);
int ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint32_t len);
int ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint32_t len);
uint32_t ring_buffer_count(ring_buffer_t *rb);
int ring_buffer_is_empty(ring_buffer_t *rb);
int ring_buffer_is_full(ring_buffer_t *rb);

#endif

