/*
 * Layer: Desktop Phase 1 - Session model
 * File: session.c
 * Purpose: Track launched apps and recent apps for the panel.
 */

#include "desktop.h"
#include <string.h>
#include "../gui/wm_core.h"

static char g_recent[5][DESKTOP_APP_NAME_MAX];
static int g_recent_pos;

int desktop_session_init(void)
{
    memset(g_recent, 0, sizeof(g_recent));
    g_recent_pos = 0;
    return 0;
}

void desktop_session_note_launch(const char *id)
{
    if (!id) return;
    strncpy(g_recent[g_recent_pos], id, DESKTOP_APP_NAME_MAX - 1);
    g_recent[g_recent_pos][DESKTOP_APP_NAME_MAX - 1] = '\0';
    g_recent_pos = (g_recent_pos + 1) % 5;
}

const char *desktop_session_recent(int index)
{
    if (index < 0 || index >= 5) return 0;
    int pos = g_recent_pos - 1 - index;
    if (pos < 0) pos += 5;
    return g_recent[pos][0] ? g_recent[pos] : 0;
}

int desktop_session_open_count(void)
{
    return wm_get_count();
}
