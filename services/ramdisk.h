/*
 * Layer: 4 (System Services) - RAM Disk
 * File: ramdisk.h
 * Purpose: In-memory filesystem for VFS testing
 */

#ifndef RAMDISK_H
#define RAMDISK_H

#include <types.h>
#include "vfs.h"

#define RAMDISK_BLOCK_SIZE  4096
#define RAMDISK_MAX_FILES   32
#define RAMDISK_MAX_BLOCKS  256

int ramdisk_init(void);
int ramdisk_mount(const char *device, vfs_inode_t *mount_point);
int ramdisk_create_file(const char *path, const uint8_t *data, uint32_t len);

#endif

