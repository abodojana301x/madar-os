/*
 * Layer: 7 (User Applications) - Graphical File Manager core
 * File: fileman.c
 * Purpose: Lightweight VFS browser model used by GUI or serial fallback.
 */

#include "fileman.h"
#include "kprintf.h"
#include "string.h"
#include "../services/vfs.h"
#include "../services/ramdisk.h"

int fileman_init(void) {
    vfs_mkdir("/home");
    vfs_mkdir("/home/guest");
    ramdisk_create_file("/home/guest/readme.txt", (const uint8_t *)"Welcome to Madar", 16);
    kprintf("File Manager initialized\n");
    return 0;
}

int fileman_copy(const char *src, const char *dst) {
    uint8_t buf[512];
    int fd = vfs_open(src, VFS_O_RDONLY);
    if (fd < 0) return -1;
    int n = vfs_read(fd, buf, sizeof(buf));
    vfs_close(fd);
    if (n < 0) return -1;
    return vfs_create_file(dst, buf, (uint32_t)n);
}

int fileman_selftest(void) {
    char name[VFS_MAX_NAME_LEN];
    int listed = 0;
    kprintf("Testing File Manager...\n");
    if (vfs_readdir("/home/guest", 0, name) == 0) listed = 1;
    if (!listed) return -1;
    if (fileman_copy("/home/guest/readme.txt", "/home/guest/readme-copy.txt") != 0) return -1;
    if (vfs_open("/home/guest/readme-copy.txt", VFS_O_RDONLY) < 0) return -1;
    kprintf("File Manager PASSED\n");
    return 0;
}
