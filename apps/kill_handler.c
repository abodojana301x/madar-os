#include "kprintf.h"

int kill_handler_request(unsigned pid) {
    if (pid == 0 || pid == 1) return -1;
    kprintf("taskmgr: SIGTERM pid=%u (simulated safe path)\n", pid);
    return 0;
}
