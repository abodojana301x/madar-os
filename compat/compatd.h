#ifndef COMPAT_COMPATD_H
#define COMPAT_COMPATD_H

#include <types.h>

void compatd_init(void);
int compatd_exec_exe(const char *path);
int compatd_layer5_selftest(void);

#endif
