#include "desktop.h"
#include "../gui/gfx_backend.h"
#include "../gui/font_engine.h"
#include "../gui/gfx_utils.h"
#include "../gui/widgets.h"
#include "../gui/wm_core.h"
#include <timer.h>

#define PANEL_H 48
#define TIMER_FREQUENCY 100

static void ticks_to_time_str(uint64_t ticks, char *out)
{
    if (!out) return;
    uint64_t total_seconds = ticks / TIMER_FREQUENCY;
    uint64_t hours = (total_seconds / 3600) % 24;
    uint64_t minutes = (total_seconds / 60) % 60;
    out[0] = (char)('0' + (hours / 10));
    out[1] = (char)('0' + (hours % 10));
    out[2] = ':';
    out[3] = (char)('0' + (minutes / 10));
    out[4] = (char)('0' + (minutes % 10));
    out[5] = '\0';
}

int desktop_panel_init(void)
{
    return 0;
}

void desktop_panel_draw(void)
{
    uint32_t w = gfx_get_width();
    uint32_t h = gfx_get_height();
    if (h < PANEL_H) return;

    int y = (int)(h - PANEL_H);

    gfx_fill_rect(0, y, (int)w, PANEL_H, 0x00101524);
    gfx_fill_rect(0, y, (int)w, 1, 0x004A6A9B);

    int center_area = 480;
    int cx = ((int)w - center_area) / 2;

    gfx_draw_shadow(cx - 16, y + 3, center_area + 32, 40, 14, 0x00000000, 72);
    gfx_fill_rect_rounded_blend(cx - 14, y + 5, center_area + 28, 36, 18, 0xCC283752);
    gfx_fill_rect_rounded_blend(cx - 12, y + 7, center_area + 24, 32, 16, 0xCC344868);
    gfx_fill_rect_rounded_blend(cx - 10, y + 8, center_area + 20, 8, 14, 0xCC6B87B4);

    widget_draw_button(cx, y + 10, 56, 26, "START", desktop_start_menu_is_open());
    widget_draw_button(cx + 66, y + 10, 48, 26, "TERM", 0);
    widget_draw_button(cx + 122, y + 10, 48, 26, "FILES", 0);
    widget_draw_button(cx + 178, y + 10, 48, 26, "EDGE", 0);
    widget_draw_button(cx + 234, y + 10, 48, 26, "SET", 0);
    widget_draw_button(cx + 290, y + 10, 48, 26, "TASKS", 0);
    widget_draw_button(cx + 350, y + 10, 48, 26, "LAUNCH", 0);
    widget_draw_button(cx + 406, y + 10, 48, 26, "CLIP", 0);

    font_draw_text(12, y + 18, "MADAR OS", 0x00E0EEFF, 0);

    char count[8];
    int c = wm_get_count();
    if (c > 0) {
        count[0] = (char)('0' + c);
        count[1] = '\0';
        font_draw_text(88, y + 18, count, 0x00FFFFFF, 0);
        font_draw_text(100, y + 18, "windows", 0x009AACCC, 0);
    }

    char time_str[10];
    ticks_to_time_str(timer_get_ticks(), time_str);

    int clock_w = 90;
    int clock_x = (int)w - clock_w - 16;
    gfx_fill_rect_rounded_blend(clock_x, y + 8, clock_w, 30, 10, 0xCC1C2A44);
    font_draw_text_aligned(clock_x, y + 16, clock_w, time_str, 0x00FFFFFF, FONT_ALIGN_CENTER);
}

int desktop_panel_handle_click(int x, int y)
{
    uint32_t h = gfx_get_height();
    if ((uint32_t)y < h - PANEL_H) return -1;

    int center_area = 480;
    int cx = ((int)gfx_get_width() - center_area) / 2;

    if (x >= cx && x < cx + 56) {
        desktop_start_menu_toggle();
        return 0;
    }
    return 0;
}
