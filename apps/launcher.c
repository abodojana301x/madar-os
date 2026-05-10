#include "kprintf.h"
#include "string.h"

static char recent_apps[5][32];
static int recent_pos;

int launcher_start(const char *app_name) {
    if (!app_name || !app_name[0]) return -1;
    strncpy(recent_apps[recent_pos], app_name, sizeof(recent_apps[0]) - 1);
    recent_apps[recent_pos][sizeof(recent_apps[0]) - 1] = '\0';
    recent_pos = (recent_pos + 1) % 5;
    kprintf("launcher: %s ready\n", app_name);
    return 0;
}
