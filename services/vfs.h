/*
 * Layer: 4 (System Services) - Virtual File System
 * File: vfs.h
 * Purpose: Unified filesystem interface
 */

#ifndef VFS_H
#define VFS_H

#include <types.h>
#include "security.h"

#define VFS_MAX_FILES       256
#define VFS_MAX_OPEN_FILES  64
#define VFS_MAX_NAME_LEN    128
#define VFS_MAX_PATH_LEN    512

/* File types */
#define VFS_TYPE_FILE       0x01
#define VFS_TYPE_DIR        0x02
#define VFS_TYPE_DEV        0x03
#define VFS_TYPE_PIPE       0x04

/* Open flags */
#define VFS_O_RDONLY        0x01
#define VFS_O_WRONLY        0x02
#define VFS_O_RDWR          0x03
#define VFS_O_CREAT         0x04
#define VFS_O_APPEND        0x08
#define VFS_O_TRUNC         0x10

/* Seek origins */
#define VFS_SEEK_SET        0
#define VFS_SEEK_CUR        1
#define VFS_SEEK_END        2

/* VFS inode structure */
typedef struct vfs_inode {
    uint32_t ino;
    uint32_t type;
    uint32_t size;
    char name[VFS_MAX_NAME_LEN];
    uint32_t ref_count;
    security_ctx_t sec;
    void *fs_data;          /* Filesystem-specific data */
    struct vfs_inode *parent;
    struct vfs_inode *next_sibling;
    struct vfs_inode *first_child;
} vfs_inode_t;

/* Open file descriptor */
typedef struct {
    int used;
    int flags;
    uint32_t offset;
    vfs_inode_t *inode;
} vfs_file_t;

/* Filesystem operations */
typedef struct {
    const char *name;
    int (*mount)(const char *device, vfs_inode_t *mount_point);
    int (*unmount)(vfs_inode_t *mount_point);
    int (*read)(vfs_inode_t *inode, uint32_t offset, uint8_t *buf, uint32_t len);
    int (*write)(vfs_inode_t *inode, uint32_t offset, const uint8_t *buf, uint32_t len);
    int (*create)(vfs_inode_t *parent, const char *name, uint32_t type);
    int (*unlink)(vfs_inode_t *parent, const char *name);
    int (*readdir)(vfs_inode_t *dir, uint32_t idx, char *name_out, vfs_inode_t **inode_out);
} vfs_ops_t;

/* VFS API */
void vfs_init(void);
int vfs_mount(const char *fs_name, const char *device, const char *mount_path);
int vfs_open(const char *path, int flags);
int vfs_read(int fd, uint8_t *buf, uint32_t len);
int vfs_write(int fd, const uint8_t *buf, uint32_t len);
int vfs_close(int fd);
int vfs_seek(int fd, int32_t offset, int whence);
int vfs_mkdir(const char *path);
int vfs_create_file(const char *path, const uint8_t *data, uint32_t len);
int vfs_readdir(const char *path, uint32_t idx, char *name_out);

/* Registration */
int vfs_register_fs(vfs_ops_t *ops);

#endif
