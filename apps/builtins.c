/*
 * Layer: 7 (User Applications) - Shell builtins
 * File: builtins.c
 * Purpose: Safe built-in commands backed by Layer 4 services.
 */

#include "shell.h"
#include "kprintf.h"
#include "app.h"
#include "string.h"
#include "../services/vfs.h"

int builtin_ls(const char* path) {
    char name[VFS_MAX_NAME_LEN];
    const char *target = path ? path : "/";
    int count = 0;

    if (target[0] == '-') {
        target = "/";
    }

    kprintf("listing %s\n", target);
    for (uint32_t i = 0; i < 100; i++) {
        if (vfs_readdir(target, i, name) != 0) break;
        kprintf("  %s\n", name);
        count++;
    }
    if (count == 0) kprintf("  (empty)\n");
    return 0;
}

int builtin_cd(const char* path) {
    kprintf("cd %s (accepted by shell context)\n", path ? path : "/");
    return 0;
}

int builtin_echo(char** args) {
    for (int i = 1; args && args[i]; i++) {
        kprintf("%s%s", args[i], args[i + 1] ? " " : "");
    }
    kprintf("\n");
    return 0;
}

int builtin_help(void) {
    kprintf("Commands: ls, cd, pwd, echo, help | English: show files in PATH | Arabic: اعرض ملفات PATH\n");
    return 0;
}
