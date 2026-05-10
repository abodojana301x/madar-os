#include "wm_core.h"
#include "gfx_backend.h"
#include "gfx_utils.h"
#include "font_engine.h"
#include "theme.h"
#include <string.h>

extern void wm_compact_z(wm_window_t *arr, int n);

static wm_window_t g_windows[WM_MAX_WINDOWS];
static uint32_t g_next_id = 1;

static void draw_minimize_glyph(int x, int y, int size, uint32_t color)
{
    int width = size / 3;
    if (width < 10) width = 10;
    gfx_fill_rect(x + (size - width) / 2, y + size / 2 + 4, width, 2, color);
}

static void draw_maximize_glyph(int x, int y, int size, uint32_t color)
{
    int box = size / 3;
    if (box < 10) box = 10;
    gfx_stroke_rect_rounded(x + (size - box) / 2, y + (size - box) / 2, box, box, 2, 1, color);
}

static void draw_close_glyph(int x, int y, int size, uint32_t color)
{
    int pad = size / 3;
    gfx_draw_line(x + pad, y + pad, x + size - pad, y + size - pad, color, 2);
    gfx_draw_line(x + size - pad, y + pad, x + pad, y + size - pad, color, 2);
}

void wm_init(void)
{
    memset(g_windows, 0, sizeof(g_windows));
    g_next_id = 1;
}

wm_window_t *wm_get_by_id(uint32_t id)
{
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].used && g_windows[i].id == id) return &g_windows[i];
    }
    return 0;
}

int wm_get_count(void)
{
    int c = 0;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) if (g_windows[i].used) c++;
    return c;
}

int wm_create(const char *title, int x, int y, int w, int h)
{
    if (w < 80 || h < 50) return -1;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (!g_windows[i].used) {
            g_windows[i].used = 1;
            g_windows[i].id = g_next_id++;
            g_windows[i].x = x;
            g_windows[i].y = y;
            g_windows[i].w = w;
            g_windows[i].h = h;
            g_windows[i].state = WM_NORMAL;
            g_windows[i].opacity = 255;
            g_windows[i].z_index = wm_get_count();
            g_windows[i].is_glass = 0;
            g_windows[i].corner_radius = 12;
            memset(g_windows[i].title, 0, WM_TITLE_MAX);
            if (title) strncpy(g_windows[i].title, title, WM_TITLE_MAX - 1);
            return (int)g_windows[i].id;
        }
    }
    return -1;
}

int wm_create_glass(const char *title, int x, int y, int w, int h)
{
    int id = wm_create(title, x, y, w, h);
    if (id > 0) {
        wm_window_t *win = wm_get_by_id((uint32_t)id);
        if (win) {
            win->is_glass = 1;
            win->opacity = 220;
        }
    }
    return id;
}

int wm_move(uint32_t id, int x, int y)
{
    wm_window_t *w = wm_get_by_id(id);
    if (!w) return -1;
    w->x = x;
    w->y = y;
    return 0;
}

int wm_resize(uint32_t id, int w, int h)
{
    wm_window_t *win = wm_get_by_id(id);
    if (!win || w < 80 || h < 50) return -1;
    if (win->state == WM_MAXIMIZED) win->state = WM_NORMAL;
    win->w = w;
    win->h = h;
    return 0;
}

int wm_minimize(uint32_t id)
{
    wm_window_t *w = wm_get_by_id(id);
    if (!w) return -1;
    if (w->state == WM_MINIMIZED) return 0;
    w->state = WM_MINIMIZED;
    return 0;
}

int wm_maximize(uint32_t id)
{
    wm_window_t *w = wm_get_by_id(id);
    if (!w) return -1;
    if (w->state == WM_MAXIMIZED) {
        w->state = WM_NORMAL;
        w->x = w->x > 0 ? w->x : 100;
        w->y = w->y > 40 ? w->y : 100;
        w->w = 400;
        w->h = 300;
    } else {
        w->state = WM_MAXIMIZED;
        w->x = 12;
        w->y = 12;
        w->w = (int)gfx_get_width() - 24;
        w->h = (int)gfx_get_height() - 72;
    }
    return 0;
}

int wm_close(uint32_t id)
{
    wm_window_t *w = wm_get_by_id(id);
    if (!w) return -1;
    w->used = 0;
    wm_compact_z(g_windows, WM_MAX_WINDOWS);
    return 0;
}

int wm_raise(uint32_t id)
{
    wm_window_t *w = wm_get_by_id(id);
    if (!w) return -1;
    int maxz = 0;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].used && g_windows[i].z_index > maxz) maxz = g_windows[i].z_index;
    }
    w->z_index = maxz + 1;
    wm_compact_z(g_windows, WM_MAX_WINDOWS);
    return 0;
}

int wm_window_at(int x, int y, uint32_t *id_out)
{
    int best = -1;
    int bestz = -1;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        wm_window_t *w = &g_windows[i];
        if (!w->used) continue;
        if (w->state == WM_MINIMIZED) continue;
        if (x >= w->x && y >= w->y && x < (w->x + w->w) && y < (w->y + w->h)) {
            if (w->z_index > bestz) {
                bestz = w->z_index;
                best = i;
            }
        }
    }
    if (best < 0) return -1;
    if (id_out) *id_out = g_windows[best].id;
    return 0;
}

int wm_window_at_titlebar(int x, int y, uint32_t *id_out)
{
    int best = -1;
    int bestz = -1;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        wm_window_t *w = &g_windows[i];
        if (!w->used) continue;
        if (w->state == WM_MINIMIZED) continue;
        int titlebar_h = 34;
        if (x >= w->x && x < (w->x + w->w) && y >= w->y && y < (w->y + titlebar_h)) {
            if (w->z_index > bestz) {
                bestz = w->z_index;
                best = i;
            }
        }
    }
    if (best < 0) return -1;
    if (id_out) *id_out = g_windows[best].id;
    return 0;
}

static void draw_window(const wm_window_t *w)
{
    int maxz = -1;
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].used && g_windows[i].z_index > maxz) maxz = g_windows[i].z_index;
    }
    int is_active = (w->z_index == maxz);
    int titlebar_height = 34;
    int radius = w->corner_radius;
    if (w->state == WM_MAXIMIZED) radius = 8;

    int button_size = titlebar_height > 28 ? titlebar_height - 8 : titlebar_height - 4;
    int button_y = w->y + (titlebar_height - button_size) / 2;
    int close_x = w->x + w->w - button_size - 10;
    int maximize_x = close_x - button_size;
    int minimize_x = maximize_x - button_size;

    gfx_draw_shadow(w->x - 6, w->y - 8, w->w + 12, w->h + 16, 16, 0x00000000, is_active ? 92 : 56);

    uint32_t window_bg = is_active ? 0xF0F8FBFF : 0xF0F0F4F9;
    uint32_t titlebar_color = is_active ? 0xF0FCFEFF : 0xF0EDF2F8;
    uint32_t border_color = is_active ? 0x00C9D7EA : 0x00D2DBE6;

    if (w->is_glass) {
        uint32_t glass_bg = 0xB0101825;
        uint32_t glass_titlebar = 0xD0151E30;
        border_color = is_active ? 0x804A6A9B : 0x60304060;
        gfx_fill_rect_rounded_blend(w->x, w->y, w->w, w->h, radius, glass_bg);
        gfx_fill_rect_rounded_blend(w->x + 1, w->y + 1, w->w - 2, titlebar_height + 12, radius - 1, glass_titlebar);
    } else {
        gfx_fill_rect_rounded(w->x, w->y, w->w, w->h, radius, window_bg);
        gfx_fill_rect_rounded(w->x + 1, w->y + 1, w->w - 2, titlebar_height + 12, radius - 1, titlebar_color);
        gfx_fill_rect_rounded(w->x + 1, w->y + 1, w->w - 2, 12, radius - 2, 0x00FFFFFF);
    }

    gfx_stroke_rect_rounded(w->x, w->y, w->w, w->h, radius, 1, border_color);
    gfx_fill_rect(w->x + 1, w->y + titlebar_height, w->w - 2, 1, 0x00DDE6F0);

    font_draw_text(w->x + 18,
                   w->y + (titlebar_height - font_get_height()) / 2,
                   w->title, is_active ? 0x00162032 : 0x006A7A8C, 0);

    gfx_fill_rect_rounded(minimize_x, button_y, button_size, button_size, 6, 0x00EEF4FB);
    gfx_fill_rect_rounded(maximize_x, button_y, button_size, button_size, 6, 0x00EEF4FB);
    gfx_fill_rect_rounded(close_x, button_y, button_size, button_size, 6,
                          is_active ? 0x00E24A59 : 0x00E07C87);

    draw_minimize_glyph(minimize_x, button_y, button_size, 0x00465C7B);
    draw_maximize_glyph(maximize_x, button_y, button_size, 0x00465C7B);
    draw_close_glyph(close_x, button_y, button_size, 0x00FFFFFF);
}

void wm_draw_all(void)
{
    for (int z = 0; z <= WM_MAX_WINDOWS; z++) {
        for (int i = 0; i < WM_MAX_WINDOWS; i++) {
            if (g_windows[i].used && g_windows[i].z_index == z && g_windows[i].state != WM_MINIMIZED) {
                draw_window(&g_windows[i]);
            }
        }
    }
}

int wm_check_button_hit(int x, int y, uint32_t win_id, int *button_type)
{
    wm_window_t *w = wm_get_by_id(win_id);
    if (!w) return -1;

    int titlebar_height = 34;
    int button_size = titlebar_height > 28 ? titlebar_height - 8 : titlebar_height - 4;
    int button_y = w->y + (titlebar_height - button_size) / 2;
    int close_x = w->x + w->w - button_size - 10;
    int maximize_x = close_x - button_size;
    int minimize_x = maximize_x - button_size;

    if (x >= minimize_x && x < minimize_x + button_size && y >= button_y && y < button_y + button_size) {
        *button_type = 1;
        return 0;
    }
    if (x >= maximize_x && x < maximize_x + button_size && y >= button_y && y < button_y + button_size) {
        *button_type = 2;
        return 0;
    }
    if (x >= close_x && x < close_x + button_size && y >= button_y && y < button_y + button_size) {
        *button_type = 3;
        return 0;
    }
    return -1;
}
