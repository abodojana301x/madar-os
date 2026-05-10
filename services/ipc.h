/*
 * Layer: 4 (System Services) - Inter-Process Communication
 * File: ipc.h
 * Purpose: Pipes and message queues between processes
 */

#ifndef IPC_H
#define IPC_H

#include <types.h>
#include "ring_buffer.h"

#define IPC_MAX_PIPES       32
#define IPC_MAX_MSGQS       16
#define IPC_MAX_MSG_SIZE    256

/* Pipe structure */
typedef struct {
    int used;
    int read_end;       /* File descriptor for reading */
    int write_end;      /* File descriptor for writing */
    ring_buffer_t buffer;
    spinlock_t lock;
} pipe_t;

/* Message queue structure */
typedef struct {
    int used;
    uint32_t key;
    uint8_t messages[IPC_MAX_MSGQS][IPC_MAX_MSG_SIZE];
    uint32_t msg_lens[IPC_MAX_MSGQS];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    spinlock_t lock;
} msg_queue_t;

void ipc_init(void);

/* Pipe API */
int pipe_create(int pipefd[2]);
int pipe_write(int fd, const uint8_t *data, uint32_t len);
int pipe_read(int fd, uint8_t *data, uint32_t len);
int pipe_close(int fd);

/* Message Queue API */
int msgq_create(uint32_t key);
int msgq_send(int msqid, const uint8_t *msg, uint32_t len);
int msgq_recv(int msqid, uint8_t *msg, uint32_t max_len);
int msgq_destroy(int msqid);

#endif
