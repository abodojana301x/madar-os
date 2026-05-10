/*
 * Layer: 4 (System Services) - Virtual File System Implementation
 * File: vfs.c
 * Purpose: Unified filesystem interface
 */

#include "vfs.h"
#include <kprintf.h>
#include <string.h>
#include <heap.h>
#include <spinlock.h>

static vfs_inode_t *root_inode = NULL;
static vfs_file_t open_files[VFS_MAX_OPEN_FILES];
static vfs_ops_t *registered_fs[8];
static int fs_count = 0;
static spinlock_t vfs_lock;
static uint32_t next_ino = 1;

/* Internal helpers */
static vfs_inode_t *alloc_inode(uint32_t type) {
    vfs_inode_t *ino = (vfs_inode_t *)kmalloc(sizeof(vfs_inode_t));
    if (!ino) return NULL;
    memset(ino, 0, sizeof(vfs_inode_t));
    ino->ino = next_ino++;
    ino->type = type;
    ino->ref_count = 1;
    ino->sec.uid = ROOT_UID;
    ino->sec.gid = ROOT_GID;
    ino->sec.mode = 0755;
    ino->name[0] = '\0';
    return ino;
}

static void free_inode(vfs_inode_t *ino) {
    if (ino && --ino->ref_count == 0) {
        kfree(ino);
    }
}

static vfs_inode_t *lookup_path(const char *path) {
    if (!path || path[0] != '/') return NULL;
    if (path[1] == '\0') return root_inode;

    vfs_inode_t *cur = root_inode;
    char comp[VFS_MAX_NAME_LEN];
    const char *p = path + 1;

    while (*p) {
        /* Extract next component */
        int i = 0;
        while (*p && *p != '/' && i < VFS_MAX_NAME_LEN - 1) {
            comp[i++] = *p++;
        }
        comp[i] = '\0';
        while (*p == '/') p++;

        if (comp[0] == '\0' || (comp[0] == '.' && comp[1] == '\0')) {
            continue;
        }
        if (comp[0] == '.' && comp[1] == '.' && comp[2] == '\0') {
            if (cur->parent) cur = cur->parent;
            continue;
        }

        /* Search children */
        vfs_inode_t *child = cur->first_child;
        vfs_inode_t *found = NULL;
        while (child) {
            const char *child_name = child->name[0] ? child->name : (const char *)child->fs_data;
            if (child_name && strcmp(child_name, comp) == 0) {
                found = child;
                break;
            }
            child = child->next_sibling;
        }
        if (!found) return NULL;
        cur = found;
    }
    return cur;
}

static int alloc_fd(void) {
    for (int i = 0; i < VFS_MAX_OPEN_FILES; i++) {
        if (!open_files[i].used) {
            open_files[i].used = 1;
            open_files[i].offset = 0;
            return i;
        }
    }
    return -1;
}

static void free_fd(int fd) {
    if (fd >= 0 && fd < VFS_MAX_OPEN_FILES) {
        if (open_files[fd].inode) {
            free_inode(open_files[fd].inode);
        }
        memset(&open_files[fd], 0, sizeof(vfs_file_t));
    }
}

/* Public API */
void vfs_init(void) {
    kprintf("📁 Initializing VFS...\n");
    spinlock_init(&vfs_lock);
    memset(open_files, 0, sizeof(open_files));
    memset(registered_fs, 0, sizeof(registered_fs));

    root_inode = alloc_inode(VFS_TYPE_DIR);
    if (!root_inode) {
        kprintf("❌ Failed to create root inode\n");
        return;
    }
    root_inode->sec.mode = 0755;
    kprintf("✅ VFS initialized (root inode=%u)\n", root_inode->ino);
}

int vfs_register_fs(vfs_ops_t *ops) {
    if (!ops || fs_count >= 8) return -1;
    registered_fs[fs_count++] = ops;
    kprintf("  Registered FS: %s\n", ops->name);
    return 0;
}

int vfs_mount(const char *fs_name, const char *device, const char *mount_path) {
    (void)device;
    if (!fs_name || !mount_path) return -1;

    vfs_ops_t *ops = NULL;
    for (int i = 0; i < fs_count; i++) {
        if (registered_fs[i] && strcmp(registered_fs[i]->name, fs_name) == 0) {
            ops = registered_fs[i];
            break;
        }
    }
    if (!ops) return -1;

    /* Find or create mount point */
    vfs_inode_t *mnt = lookup_path(mount_path);
    if (!mnt) {
        /* Create directory */
        vfs_mkdir(mount_path);
        mnt = lookup_path(mount_path);
    }
    if (!mnt) return -1;

    if (ops->mount) {
        return ops->mount(device, mnt);
    }
    return 0;
}

int vfs_open(const char *path, int flags) {
    spinlock_lock(&vfs_lock);

    vfs_inode_t *ino = lookup_path(path);
    if (!ino) {
        if ((flags & VFS_O_CREAT) && strchr(path, '/') != NULL) {
            /* Simple create: add inode under parent */
            /* For ramdisk, this is handled by ramdisk layer */
        }
        spinlock_unlock(&vfs_lock);
        return -1;
    }

    uint32_t req_perm = 0;
    if (flags & VFS_O_RDONLY) req_perm |= PERM_READ;
    if (flags & VFS_O_WRONLY) req_perm |= PERM_WRITE;
    if (flags & VFS_O_RDWR)   req_perm |= PERM_READ | PERM_WRITE;

    if (security_check(&ino->sec, req_perm) != 0) {
        kprintf("🚫 Permission denied: %s\n", path);
        spinlock_unlock(&vfs_lock);
        return -1;
    }

    int fd = alloc_fd();
    if (fd < 0) {
        spinlock_unlock(&vfs_lock);
        return -1;
    }

    open_files[fd].flags = flags;
    open_files[fd].inode = ino;
    ino->ref_count++;

    if (flags & VFS_O_TRUNC) {
        ino->size = 0;
    }

    spinlock_unlock(&vfs_lock);
    return fd;
}

int vfs_read(int fd, uint8_t *buf, uint32_t len) {
    if (fd < 0 || fd >= VFS_MAX_OPEN_FILES || !open_files[fd].used) return -1;
    if (!buf || len == 0) return -1;

    vfs_file_t *file = &open_files[fd];
    if (!(file->flags & (VFS_O_RDONLY | VFS_O_RDWR))) return -1;

    vfs_inode_t *ino = file->inode;
    if (!ino) return -1;

    /* If filesystem has read op, use it */
    /* For now, fallback to ramdisk internal read via fs_data */
    if (ino->type != VFS_TYPE_FILE) return -1;
    uint8_t *data = (uint8_t *)ino->fs_data;
    if (data) {
        uint32_t avail = ino->size - file->offset;
        if (len > avail) len = avail;
        memcpy(buf, data + file->offset, len);
        file->offset += len;
        return (int)len;
    }

    return -1;
}

int vfs_write(int fd, const uint8_t *buf, uint32_t len) {
    if (fd < 0 || fd >= VFS_MAX_OPEN_FILES || !open_files[fd].used) return -1;
    if (!buf || len == 0) return -1;

    vfs_file_t *file = &open_files[fd];
    if (!(file->flags & (VFS_O_WRONLY | VFS_O_RDWR))) return -1;

    vfs_inode_t *ino = file->inode;
    if (!ino) return -1;

    if (ino->type != VFS_TYPE_FILE) return -1;
    uint8_t *data = (uint8_t *)ino->fs_data;
    if (data) {
        uint32_t max_write = 4096 - file->offset; /* Assume 4KB buffer for ramdisk */
        if (len > max_write) len = max_write;
        memcpy(data + file->offset, buf, len);
        file->offset += len;
        if (file->offset > ino->size) {
            ino->size = file->offset;
        }
        return (int)len;
    }

    return -1;
}

int vfs_close(int fd) {
    if (fd < 0 || fd >= VFS_MAX_OPEN_FILES || !open_files[fd].used) return -1;
    free_fd(fd);
    return 0;
}

int vfs_seek(int fd, int32_t offset, int whence) {
    if (fd < 0 || fd >= VFS_MAX_OPEN_FILES || !open_files[fd].used) return -1;

    vfs_file_t *file = &open_files[fd];
    int32_t new_off = (int32_t)file->offset;

    switch (whence) {
        case VFS_SEEK_SET: new_off = offset; break;
        case VFS_SEEK_CUR: new_off += offset; break;
        case VFS_SEEK_END: new_off = (int32_t)file->inode->size + offset; break;
        default: return -1;
    }

    if (new_off < 0) new_off = 0;
    file->offset = (uint32_t)new_off;
    return 0;
}

int vfs_mkdir(const char *path) {
    if (!path || path[0] != '/') return -1;

    /* Find parent path */
    char parent_path[VFS_MAX_PATH_LEN];
    char name[VFS_MAX_NAME_LEN];
    strncpy(parent_path, path, VFS_MAX_PATH_LEN - 1);
    parent_path[VFS_MAX_PATH_LEN - 1] = '\0';

    char *last_slash = strrchr(parent_path, '/');
    if (!last_slash) return -1;
    strncpy(name, last_slash + 1, VFS_MAX_NAME_LEN - 1);
    name[VFS_MAX_NAME_LEN - 1] = '\0';

    if (last_slash == parent_path) {
        parent_path[1] = '\0';
    } else {
        *last_slash = '\0';
    }

    vfs_inode_t *parent = lookup_path(parent_path);
    if (!parent) return -1;

    vfs_inode_t *child = alloc_inode(VFS_TYPE_DIR);
    if (!child) return -1;

    strncpy(child->name, name, VFS_MAX_NAME_LEN - 1);
    child->name[VFS_MAX_NAME_LEN - 1] = '\0';
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;

    return 0;
}

int vfs_create_file(const char *path, const uint8_t *data, uint32_t len) {
    if (!path || path[0] != '/') return -1;

    char parent_path[VFS_MAX_PATH_LEN];
    char name[VFS_MAX_NAME_LEN];
    strncpy(parent_path, path, VFS_MAX_PATH_LEN - 1);
    parent_path[VFS_MAX_PATH_LEN - 1] = '\0';

    char *last_slash = strrchr(parent_path, '/');
    if (!last_slash) return -1;
    strncpy(name, last_slash + 1, VFS_MAX_NAME_LEN - 1);
    name[VFS_MAX_NAME_LEN - 1] = '\0';
    if (name[0] == '\0') return -1;

    if (last_slash == parent_path) {
        parent_path[1] = '\0';
    } else {
        *last_slash = '\0';
    }

    vfs_inode_t *parent = lookup_path(parent_path);
    if (!parent || parent->type != VFS_TYPE_DIR) return -1;

    vfs_inode_t *existing = lookup_path(path);
    if (existing && existing->type == VFS_TYPE_FILE) {
        if (existing->fs_data) kfree(existing->fs_data);
        existing->fs_data = NULL;
        existing->size = len;
        if (len > 0) {
            existing->fs_data = kmalloc(len);
            if (!existing->fs_data) {
                existing->size = 0;
                return -1;
            }
            memcpy(existing->fs_data, data, len);
        }
        return 0;
    }

    vfs_inode_t *child = alloc_inode(VFS_TYPE_FILE);
    if (!child) return -1;
    strncpy(child->name, name, VFS_MAX_NAME_LEN - 1);
    child->name[VFS_MAX_NAME_LEN - 1] = '\0';
    child->size = len;
    if (len > 0) {
        child->fs_data = kmalloc(len);
        if (!child->fs_data) {
            kfree(child);
            return -1;
        }
        memcpy(child->fs_data, data, len);
    }
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    return 0;
}

int vfs_readdir(const char *path, uint32_t idx, char *name_out) {
    if (!path || !name_out) return -1;

    vfs_inode_t *dir = lookup_path(path);
    if (!dir || dir->type != VFS_TYPE_DIR) return -1;

    vfs_inode_t *child = dir->first_child;
    uint32_t i = 0;
    while (child) {
        if (i == idx) {
            const char *cname = child->name[0] ? child->name : (const char *)child->fs_data;
            if (cname) {
                strncpy(name_out, cname, VFS_MAX_NAME_LEN - 1);
                name_out[VFS_MAX_NAME_LEN - 1] = '\0';
            } else {
                name_out[0] = '\0';
            }
            return 0;
        }
        child = child->next_sibling;
        i++;
    }
    return -1; /* No more entries */
}
