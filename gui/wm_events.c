#include "wm_core.h"
#include "event_loop.h"
#include "theme.h"
#include "gfx_backend.h"

/* Drag state tracking */
static struct {
    uint32_t dragging_id;
    int drag_offset_x;
    int drag_offset_y;
    int is_dragging;
} g_drag_state = {0, 0, 0, 0};

static int titlebar_height(void)
{
    return theme_get()->spacing.title_height;
}

static int control_button_size(void)
{
    int title_h = titlebar_height();
    return title_h > 28 ? title_h - 8 : title_h - 4;
}

int wm_handle_click(int x, int y, uint32_t *focus_id)
{
    return wm_window_at(x, y, focus_id);
}

/**
 * Handle mouse down event
 * - Check if clicked on title bar (drag start)
 * - Check if clicked on close button
 * - Raise window and set focus
 */
int wm_handle_mouse_down(int x, int y, uint32_t *window_id)
{
    uint32_t wid = 0;
    
    /* Find window at click position (highest z-order) */
    if (wm_window_at(x, y, &wid) != 0) {
        return -1; /* No window clicked */
    }

    wm_window_t *w = wm_get_by_id(wid);
    if (!w) return -1;

    /* Check if close button clicked */
    int btn_size = control_button_size();
    int btn_y = w->y + (titlebar_height() - btn_size) / 2;
    int close_x = w->x + w->w - btn_size - 8;
    int max_x = close_x - btn_size;
    int min_x = max_x - btn_size;

    if (x >= close_x && x < (close_x + btn_size) &&
        y >= btn_y && y < (btn_y + btn_size)) {
        
        wm_close(wid);
        if (window_id) *window_id = 0;
        return 1; /* Window closed */
    }

    if (x >= max_x && x < (max_x + btn_size) &&
        y >= btn_y && y < (btn_y + btn_size)) {
        wm_maximize(wid);
        wm_raise(wid);
        if (window_id) *window_id = wid;
        return 0;
    }

    if (x >= min_x && x < (min_x + btn_size) &&
        y >= btn_y && y < (btn_y + btn_size)) {
        wm_minimize(wid);
        if (window_id) *window_id = wid;
        return 0;
    }

    /* Check if title bar clicked (start drag) */
    if (y >= w->y && y < (w->y + titlebar_height())) {
        g_drag_state.dragging_id = wid;
        g_drag_state.drag_offset_x = x - w->x;
        g_drag_state.drag_offset_y = y - w->y;
        g_drag_state.is_dragging = 1;
    }

    /* Raise and focus the window */
    wm_raise(wid);
    if (window_id) *window_id = wid;
    
    return 0; /* Window clicked but not closed */
}

/**
 * Handle mouse move event
 * - Update dragging window position if dragging
 */
int wm_handle_mouse_move(int x, int y)
{
    if (!g_drag_state.is_dragging) return 0;

    wm_window_t *w = wm_get_by_id(g_drag_state.dragging_id);
    if (!w) {
        g_drag_state.is_dragging = 0;
        return -1;
    }

    /* Update window position relative to drag offset */
    w->x = x - g_drag_state.drag_offset_x;
    w->y = y - g_drag_state.drag_offset_y;

    /* Bounds checking - keep window partially visible */
    int max_x = (int)gfx_get_width() - 60;
    int max_y = (int)gfx_get_height() - 60;
    if (w->x < 0) w->x = 0;
    if (w->y < 0) w->y = 0;
    if (w->x > max_x) w->x = max_x;
    if (w->y > max_y) w->y = max_y;
    if (w->x + w->w < 80) w->x = 80 - w->w;
    if (w->y + titlebar_height() < titlebar_height()) w->y = 0;

    return 0;
}

/**
 * Handle mouse up event
 * - End dragging state
 */
int wm_handle_mouse_up(void)
{
    if (!g_drag_state.is_dragging) return -1;
    
    g_drag_state.is_dragging = 0;
    g_drag_state.dragging_id = 0;
    
    return 0;
}
