#include "string.h"

int process_list_snapshot(char *buf, unsigned long size) {
    const char *snapshot = "pid name state\n1 init running\n2 gui_daemon running\n3 shell ready\n";
    if (!buf || size == 0) return -1;
    strncpy(buf, snapshot, size - 1);
    buf[size - 1] = '\0';
    return (int)strlen(buf);
}
