#ifndef COMPAT_SAFE_EXIT_H
#define COMPAT_SAFE_EXIT_H

#include <types.h>

typedef void (*compat_atexit_func_t)(void);

int compat_atexit(compat_atexit_func_t func);
void compat_cleanup_on_crash(void);
void compat_auto_close_on_exit(void);
void compat_exit_cleanup(uint32_t exit_code);
int compat_register_cleanup_path(const char *path);

#endif
