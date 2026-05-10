#include "input_system.h"
#include "wm_core.h"
#include <string.h>

static hover_state_t g_hover = {0};

void input_system_init(void)
{
    memset(&g_hover, 0, sizeof(hover_state_t));
}

void input_system_update_hover(int x, int y)
{
    g_hover.hover_x = x;
    g_hover.hover_y = y;
    
    /* Find window at this position (highest z-order) */
    uint32_t wid = 0;
    if (wm_window_at(x, y, &wid) == 0) {
        g_hover.hovered_window_id = wid;
    } else {
        g_hover.hovered_window_id = 0;
    }
}

uint32_t input_system_get_hovered_window(void)
{
    return g_hover.hovered_window_id;
}

int input_system_get_window_count(void)
{
    return wm_get_count();
}

int input_system_cycle_windows_forward(void)
{
    int wcount = wm_get_count();
    if (wcount <= 1) return -1;
    
    /* Get all windows sorted by z-index */
    wm_window_t *windows[WM_MAX_WINDOWS];
    int win_count = 0;
    
    for (int z = 0; z <= WM_MAX_WINDOWS; z++) {
        for (int i = 0; i < WM_MAX_WINDOWS; i++) {
            wm_window_t *w = wm_get_by_id(i + 1);
            if (w && w->used && w->z_index == z && w->state != WM_MINIMIZED) {
                windows[win_count++] = w;
            }
        }
    }
    
    if (win_count <= 1) return -1;
    
    /* Cycle: move last window to front */
    wm_window_t *last = windows[win_count - 1];
    
    /* Raise the last window to be the new focus */
    uint32_t last_id = last->id;
    wm_raise(last_id);
    
    extern void focus_set(uint32_t id);
    focus_set(last_id);
    
    return (int)last_id;
}

int input_system_cycle_windows_backward(void)
{
    int wcount = wm_get_count();
    if (wcount <= 1) return -1;
    
    /* Get all windows sorted by z-index (reverse order for backward cycle) */
    wm_window_t *windows[WM_MAX_WINDOWS];
    int win_count = 0;
    
    for (int z = WM_MAX_WINDOWS; z >= 0; z--) {
        for (int i = 0; i < WM_MAX_WINDOWS; i++) {
            wm_window_t *w = wm_get_by_id(i + 1);
            if (w && w->used && w->z_index == z && w->state != WM_MINIMIZED) {
                windows[win_count++] = w;
            }
        }
    }
    
    if (win_count <= 1) return -1;
    
    /* Cycle backward: move first window to back */
    wm_window_t *first = windows[0];
    
    /* Raise the first window to be the new focus */
    uint32_t first_id = first->id;
    wm_raise(first_id);
    
    extern void focus_set(uint32_t id);
    focus_set(first_id);
    
    return (int)first_id;
}
