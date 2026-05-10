#include "string.h"

static char clipboard_path[128];

void clipboard_set_path(const char *path) {
    strncpy(clipboard_path, path ? path : "", sizeof(clipboard_path) - 1);
    clipboard_path[sizeof(clipboard_path) - 1] = '\0';
}

const char *clipboard_get_path(void) {
    return clipboard_path;
}
