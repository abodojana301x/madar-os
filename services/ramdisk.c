/*
 * Layer: 4 (System Services) - RAM Disk Implementation
 * File: ramdisk.c
 * Purpose: In-memory filesystem for VFS testing
 */

#include "ramdisk.h"
#include <kprintf.h>
#include <string.h>
#include <heap.h>

typedef struct {
    uint8_t data[RAMDISK_BLOCK_SIZE];
    int used;
} ramdisk_block_t;

typedef struct {
    char name[VFS_MAX_NAME_LEN];
    uint32_t size;
    uint8_t *data;
    int used;
} ramdisk_file_t;

static ramdisk_file_t ramdisk_files[RAMDISK_MAX_FILES];
static ramdisk_block_t ramdisk_blocks[RAMDISK_MAX_BLOCKS];
static int ramdisk_initialized = 0;

int ramdisk_init(void) {
    kprintf("💾 Initializing RAM Disk...\n");
    memset(ramdisk_files, 0, sizeof(ramdisk_files));
    memset(ramdisk_blocks, 0, sizeof(ramdisk_blocks));
    ramdisk_initialized = 1;
    kprintf("✅ RAM Disk initialized (%d files max)\n", RAMDISK_MAX_FILES);
    return 0;
}

static int find_free_file(void) {
    for (int i = 0; i < RAMDISK_MAX_FILES; i++) {
        if (!ramdisk_files[i].used) return i;
    }
    return -1;
}

int ramdisk_create_file(const char *path, const uint8_t *data, uint32_t len) {
    if (!ramdisk_initialized || !path) return -1;

    int idx = find_free_file();
    if (idx < 0) return -1;

    ramdisk_file_t *f = &ramdisk_files[idx];
    strncpy(f->name, path, VFS_MAX_NAME_LEN - 1);
    f->name[VFS_MAX_NAME_LEN - 1] = '\0';
    f->size = len;
    f->used = 1;

    if (len > 0 && data) {
        f->data = (uint8_t *)kmalloc(len);
        if (f->data) {
            memcpy(f->data, data, len);
        } else {
            f->size = 0;
        }
    } else {
        f->data = NULL;
        f->size = 0;
    }

    kprintf("  RAM Disk: created '%s' (%u bytes)\n", f->name, f->size);
    (void)vfs_create_file(path, data, len);
    return 0;
}

/* VFS ops wrappers */
static int ramdisk_vfs_read(vfs_inode_t *inode, uint32_t offset, uint8_t *buf, uint32_t len) {
    ramdisk_file_t *f = (ramdisk_file_t *)inode->fs_data;
    if (!f || !f->used || !buf) return -1;

    if (offset >= f->size) return 0;
    if (offset + len > f->size) len = f->size - offset;

    memcpy(buf, f->data + offset, len);
    return (int)len;
}

static int ramdisk_vfs_write(vfs_inode_t *inode, uint32_t offset, const uint8_t *buf, uint32_t len) {
    ramdisk_file_t *f = (ramdisk_file_t *)inode->fs_data;
    if (!f || !f->used || !buf) return -1;

    if (offset + len > RAMDISK_BLOCK_SIZE) {
        len = RAMDISK_BLOCK_SIZE - offset;
    }

    if (!f->data) {
        f->data = (uint8_t *)kmalloc(RAMDISK_BLOCK_SIZE);
        if (!f->data) return -1;
        memset(f->data, 0, RAMDISK_BLOCK_SIZE);
    }

    memcpy(f->data + offset, buf, len);
    if (offset + len > f->size) {
        f->size = offset + len;
    }
    return (int)len;
}

static int ramdisk_vfs_readdir(vfs_inode_t *dir, uint32_t idx, char *name_out, vfs_inode_t **inode_out) {
    (void)dir;
    int count = 0;
    for (int i = 0; i < RAMDISK_MAX_FILES; i++) {
        if (ramdisk_files[i].used) {
            if ((uint32_t)count == idx) {
                if (name_out) {
                    strncpy(name_out, ramdisk_files[i].name, VFS_MAX_NAME_LEN - 1);
                    name_out[VFS_MAX_NAME_LEN - 1] = '\0';
                }
                if (inode_out) *inode_out = NULL; /* Simplified */
                return 0;
            }
            count++;
        }
    }
    return -1;
}

static vfs_ops_t ramdisk_ops = {
    .name = "ramdisk",
    .mount = NULL,
    .unmount = NULL,
    .read = ramdisk_vfs_read,
    .write = ramdisk_vfs_write,
    .create = NULL,
    .unlink = NULL,
    .readdir = ramdisk_vfs_readdir,
};

int ramdisk_mount(const char *device, vfs_inode_t *mount_point) {
    (void)device;
    (void)mount_point;
    vfs_register_fs(&ramdisk_ops);
    kprintf("✅ RAM Disk mounted\n");
    return 0;
}
