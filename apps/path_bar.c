#include "string.h"

void path_bar_format(const char *path, char *out, unsigned long size) {
    if (!out || size == 0) return;
    strncpy(out, path ? path : "/", size - 1);
    out[size - 1] = '\0';
}
