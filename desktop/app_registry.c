/*
 * Layer: Desktop Phase 1 - App registry
 * File: app_registry.c
 * Purpose: Register core desktop apps and launch them through the window manager.
 */

#include "desktop.h"
#include "../gui/wm_core.h"
#include <string.h>

static desktop_app_t g_apps[DESKTOP_MAX_APPS];
static int g_app_count;

int desktop_app_registry_init(void)
{
    g_apps[0] = (desktop_app_t){ "terminal", "Terminal", "Madar Terminal" };
    g_apps[1] = (desktop_app_t){ "files", "Files", "Madar Files" };
    g_apps[2] = (desktop_app_t){ "settings", "Settings", "Madar Settings" };
    g_apps[3] = (desktop_app_t){ "tasks", "Tasks", "Madar Task Manager" };
    g_app_count = 4;
    return 0;
}

int desktop_app_count(void)
{
    return g_app_count;
}

const desktop_app_t *desktop_app_get(int index)
{
    if (index < 0 || index >= g_app_count) return 0;
    return &g_apps[index];
}

const desktop_app_t *desktop_app_find(const char *id)
{
    if (!id) return 0;
    for (int i = 0; i < g_app_count; i++) {
        if (strcmp(g_apps[i].id, id) == 0) return &g_apps[i];
    }
    return 0;
}

int desktop_app_launch(const char *id)
{
    const desktop_app_t *app = desktop_app_find(id);
    if (!app) return -1;

    int slot = desktop_session_open_count();
    int x = 110 + (slot * 28);
    int y = 95 + (slot * 24);
    int wid = wm_create(app->title, x, y, 360, 220);
    if (wid < 0) return -1;

    (void)wm_raise((uint32_t)wid);
    desktop_session_note_launch(id);
    desktop_start_menu_close();
    return wid;
}
