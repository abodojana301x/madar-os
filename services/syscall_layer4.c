/*
 * Layer: 4 (System Services) - Syscall Implementations
 * File: syscall_layer4.c
 * Purpose: System call handlers for VFS, IPC, Security
 */

#include <types.h>
#include <syscalls_list.h>
#include <kprintf.h>
#include <string.h>
#include "vfs.h"
#include "ipc.h"
#include "security.h"
#include "signal.h"

uint64_t sys_open(uint64_t path_ptr, uint64_t flags, uint64_t, uint64_t, uint64_t, uint64_t) {
    const char *path = (const char *)path_ptr;
    if (!path) return (uint64_t)-1;
    return (uint64_t)vfs_open(path, (int)flags);
}

uint64_t sys_read(uint64_t fd, uint64_t buf_ptr, uint64_t len, uint64_t, uint64_t, uint64_t) {
    uint8_t *buf = (uint8_t *)buf_ptr;
    if (!buf) return (uint64_t)-1;

    int ifd = (int)fd;
    /* Check if it's a pipe fd (>=100) or VFS fd */
    if (ifd >= 100 && ifd < 300) {
        return (uint64_t)pipe_read(ifd, buf, (uint32_t)len);
    }
    return (uint64_t)vfs_read(ifd, buf, (uint32_t)len);
}

uint64_t sys_write(uint64_t fd, uint64_t buf_ptr, uint64_t len, uint64_t, uint64_t, uint64_t) {
    const uint8_t *buf = (const uint8_t *)buf_ptr;
    if (!buf) return (uint64_t)-1;

    int ifd = (int)fd;
    if (ifd >= 100 && ifd < 300) {
        return (uint64_t)pipe_write(ifd, buf, (uint32_t)len);
    }
    return (uint64_t)vfs_write(ifd, buf, (uint32_t)len);
}

uint64_t sys_close(uint64_t fd, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    int ifd = (int)fd;
    if (ifd >= 100 && ifd < 300) {
        return (uint64_t)pipe_close(ifd);
    }
    return (uint64_t)vfs_close(ifd);
}

uint64_t sys_mount(uint64_t fs_ptr, uint64_t dev_ptr, uint64_t path_ptr, uint64_t, uint64_t, uint64_t) {
    const char *fs = (const char *)fs_ptr;
    const char *dev = (const char *)dev_ptr;
    const char *path = (const char *)path_ptr;
    if (!fs || !path) return (uint64_t)-1;
    return (uint64_t)vfs_mount(fs, dev, path);
}

uint64_t sys_pipe(uint64_t pipefd_ptr, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    int *pipefd = (int *)pipefd_ptr;
    if (!pipefd) return (uint64_t)-1;
    return (uint64_t)pipe_create(pipefd);
}

uint64_t sys_kill(uint64_t pid, uint64_t sig, uint64_t, uint64_t, uint64_t, uint64_t) {
    return (uint64_t)signal_send((uint32_t)pid, (uint32_t)sig);
}

uint64_t sys_setuid(uint64_t uid, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) {
    return (uint64_t)security_set_uid((uint32_t)uid);
}

uint64_t sys_chmod(uint64_t path_ptr, uint64_t mode, uint64_t, uint64_t, uint64_t, uint64_t) {
    (void)path_ptr;
    (void)mode;
    /* Simplified - would lookup inode and set mode */
    kprintf("sys_chmod: stub\n");
    return 0;
}

