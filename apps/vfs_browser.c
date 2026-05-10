#include "fileman.h"
#include "kprintf.h"
#include "../services/vfs.h"

int vfs_browser_list(const char *path) {
    char name[VFS_MAX_NAME_LEN];
    for (uint32_t i = 0; i < 100; i++) {
        if (vfs_readdir(path, i, name) != 0) break;
        kprintf("  %s\n", name);
    }
    return 0;
}
