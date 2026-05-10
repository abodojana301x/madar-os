/*
 * Layer: 4 (System Services) - IPC Implementation
 * File: ipc.c
 * Purpose: Pipes and message queues between processes
 */

#include "ipc.h"
#include <kprintf.h>
#include <string.h>

static pipe_t pipes[IPC_MAX_PIPES];
static msg_queue_t msg_queues[IPC_MAX_MSGQS];

void ipc_init(void) {
    kprintf("📡 Initializing IPC subsystem...\n");
    memset(pipes, 0, sizeof(pipes));
    memset(msg_queues, 0, sizeof(msg_queues));
    kprintf("✅ IPC initialized\n");
}

/* Pipe implementation */
int pipe_create(int pipefd[2]) {
    if (!pipefd) return -1;

    for (int i = 0; i < IPC_MAX_PIPES; i++) {
        if (!pipes[i].used) {
            pipes[i].used = 1;
            ring_buffer_init(&pipes[i].buffer);
            spinlock_init(&pipes[i].lock);
            /* Use index+100 as fd to avoid collision with VFS fds */
            pipes[i].read_end = i + 100;
            pipes[i].write_end = i + 200;
            pipefd[0] = pipes[i].read_end;
            pipefd[1] = pipes[i].write_end;
            kprintf("  Pipe created: read=%d write=%d\n", pipefd[0], pipefd[1]);
            return 0;
        }
    }
    return -1;
}

static pipe_t *find_pipe_by_fd(int fd) {
    for (int i = 0; i < IPC_MAX_PIPES; i++) {
        if (pipes[i].used && (pipes[i].read_end == fd || pipes[i].write_end == fd)) {
            return &pipes[i];
        }
    }
    return NULL;
}

int pipe_write(int fd, const uint8_t *data, uint32_t len) {
    pipe_t *p = find_pipe_by_fd(fd);
    if (!p || !data || len == 0) return -1;
    if (p->write_end != fd) return -1; /* Not write end */

    return ring_buffer_write(&p->buffer, data, len);
}

int pipe_read(int fd, uint8_t *data, uint32_t len) {
    pipe_t *p = find_pipe_by_fd(fd);
    if (!p || !data || len == 0) return -1;
    if (p->read_end != fd) return -1; /* Not read end */

    return ring_buffer_read(&p->buffer, data, len);
}

int pipe_close(int fd) {
    pipe_t *p = find_pipe_by_fd(fd);
    if (!p) return -1;

    if (p->read_end == fd) p->read_end = -1;
    if (p->write_end == fd) p->write_end = -1;

    /* If both ends closed, free pipe */
    if (p->read_end < 0 && p->write_end < 0) {
        p->used = 0;
    }
    return 0;
}

/* Message Queue implementation */
int msgq_create(uint32_t key) {
    for (int i = 0; i < IPC_MAX_MSGQS; i++) {
        if (!msg_queues[i].used) {
            msg_queues[i].used = 1;
            msg_queues[i].key = key;
            msg_queues[i].head = 0;
            msg_queues[i].tail = 0;
            msg_queues[i].count = 0;
            spinlock_init(&msg_queues[i].lock);
            kprintf("  MsgQ created: id=%d key=%u\n", i, key);
            return i;
        }
    }
    return -1;
}

int msgq_send(int msqid, const uint8_t *msg, uint32_t len) {
    if (msqid < 0 || msqid >= IPC_MAX_MSGQS || !msg_queues[msqid].used) return -1;
    if (!msg || len == 0 || len > IPC_MAX_MSG_SIZE) return -1;

    msg_queue_t *mq = &msg_queues[msqid];
    spinlock_lock(&mq->lock);

    if (mq->count >= IPC_MAX_MSGQS) {
        spinlock_unlock(&mq->lock);
        return -1; /* Full */
    }

    uint32_t idx = mq->head;
    memcpy(mq->messages[idx], msg, len);
    mq->msg_lens[idx] = len;
    mq->head = (mq->head + 1) % IPC_MAX_MSGQS;
    mq->count++;

    spinlock_unlock(&mq->lock);
    return 0;
}

int msgq_recv(int msqid, uint8_t *msg, uint32_t max_len) {
    if (msqid < 0 || msqid >= IPC_MAX_MSGQS || !msg_queues[msqid].used) return -1;
    if (!msg || max_len == 0) return -1;

    msg_queue_t *mq = &msg_queues[msqid];
    spinlock_lock(&mq->lock);

    if (mq->count == 0) {
        spinlock_unlock(&mq->lock);
        return -1; /* Empty */
    }

    uint32_t idx = mq->tail;
    uint32_t len = mq->msg_lens[idx];
    if (len > max_len) len = max_len;
    memcpy(msg, mq->messages[idx], len);
    mq->tail = (mq->tail + 1) % IPC_MAX_MSGQS;
    mq->count--;

    spinlock_unlock(&mq->lock);
    return (int)len;
}

int msgq_destroy(int msqid) {
    if (msqid < 0 || msqid >= IPC_MAX_MSGQS) return -1;
    msg_queues[msqid].used = 0;
    return 0;
}

