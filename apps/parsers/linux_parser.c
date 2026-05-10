#include "../shell.h"
#include "../../../include/string.h"
#include "../../../include/kprintf.h"

/* Minimal POSIX parser hook used by the Layer 7 shell parser path. */
int linux_parse(const char* cmd) {
    kprintf("Linux parser: %s (ls|cd|echo supported)\n", cmd ? cmd : "");
    return 0;
}
