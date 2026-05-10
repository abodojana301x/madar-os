#include "widgets.h"
#include "gfx_backend.h"
#include "gfx_utils.h"
#include "font_engine.h"
#include "theme.h"

void widgets_init(void)
{
    theme_init();
}

int widget_hit_test(int x, int y, int wx, int wy, int ww, int wh)
{
    return (x >= wx && y >= wy && x < wx + ww && y < wy + wh) ? 1 : 0;
}

void widget_draw_button(int x, int y, int w, int h, const char *label, int active)
{
    uint32_t outer, inner, text_color;
    if (active) {
        outer = 0x00386090; inner = 0x004878B0; text_color = 0x00FFFFFF;
    } else {
        outer = 0x00D6E3F6; inner = 0x00ECF3FB; text_color = 0x0019263E;
    }
    gfx_fill_rect_rounded(x, y, w, h, 10, outer);
    gfx_fill_rect_rounded(x + 1, y + 1, w - 2, h - 2, 9, inner);
    if (!active) {
        gfx_fill_rect_rounded(x + 1, y + 1, w - 2, 8, 8, 0x00FFFFFF);
    }
    font_draw_text_aligned(x, y + (h - font_get_height()) / 2, w, label ? label : "", text_color, FONT_ALIGN_CENTER);
}

void widget_draw_button_modern(int x, int y, int w, int h, const char *label,
                               uint32_t bg_color, uint32_t text_color, int radius)
{
    gfx_fill_rect_rounded(x, y, w, h, radius, bg_color);
    font_draw_text_aligned(x, y + (h - font_get_height()) / 2, w, label ? label : "", text_color, FONT_ALIGN_CENTER);
}

void widget_draw_textfield(int x, int y, int w, int h, const char *text, int focused)
{
    uint32_t border = focused ? 0x004878B0 : 0x00304060;
    uint32_t bg = focused ? 0x00203050 : 0x00182540;
    gfx_fill_rect_rounded(x, y, w, h, 4, bg);
    gfx_stroke_rect_rounded(x, y, w, h, 4, 1, border);
    font_draw_text(x + 6, y + 4, text ? text : "", 0x00C0D0E0, 0);
}

void widget_draw_list(int x, int y, int w, int h, const char **items, int count, int selected)
{
    gfx_fill_rect_rounded(x, y, w, h, 6, 0x00182540);
    gfx_stroke_rect_rounded(x, y, w, h, 6, 1, 0x00304060);
    int item_h = 22;
    for (int i = 0; i < count && i < 20; i++) {
        uint32_t bg = (i == selected) ? 0x00406890 : 0x001C2C48;
        gfx_fill_rect(x + 2, y + 2 + i * item_h, w - 4, item_h, bg);
        if (items[i]) {
            font_draw_text(x + 8, y + 4 + i * item_h, items[i], 0x00C0D8F0, 0);
        }
    }
}

void widget_draw_menu(int x, int y, int w, int h, const char **items, int count, int selected)
{
    gfx_fill_rect_rounded(x, y, w, h, 8, 0xF0202838);
    gfx_stroke_rect_rounded(x, y, w, h, 8, 1, 0x00405070);
    gfx_fill_rect_rounded(x + 2, y + 2, w - 4, 6, 4, 0x004878B0);
    int item_h = 24;
    for (int i = 0; i < count && i < 16; i++) {
        uint32_t bg = (i == selected) ? 0x00406890 : 0x00202838;
        gfx_fill_rect(x + 2, y + 8 + i * item_h, w - 4, item_h, bg);
        if (items[i]) {
            font_draw_text(x + 10, y + 10 + i * item_h, items[i], 0x00C0D8F0, 0);
        }
    }
}

void widget_draw_dialog(int x, int y, int w, int h, const char *title, const char *message)
{
    gfx_draw_shadow(x - 8, y - 10, w + 16, h + 20, 16, 0x00000000, 80);
    gfx_fill_rect_rounded(x, y, w, h, 12, 0xE01C2C40);
    gfx_fill_rect_rounded(x, y, w, 26, 12, 0xE0305070);
    gfx_fill_rect_rounded(x + 2, y + 2, w - 4, 10, 10, 0xE0406080);
    font_draw_text(x + 12, y + 6, title ? title : "Dialog", 0x00FFFFFF, 0);
    gfx_fill_rect_rounded(x + w - 22, y + 5, 16, 16, 8, 0x00904040);
    font_draw_text(x + w - 18, y + 8, "X", 0x00FFFFFF, 0);
    if (message) {
        font_draw_text(x + 12, y + 36, message, 0x00B0C8E0, 0);
    }
    gfx_fill_rect_rounded(x + w/2 - 40, y + h - 30, 80, 22, 8, 0x004878B0);
    font_draw_text_aligned(x + w/2 - 40, y + h - 28, 80, "OK", 0x00FFFFFF, FONT_ALIGN_CENTER);
}

int widget_list_hittest(int x, int y, int items_y, int item_h, int count)
{
    if (x < 0 || y < items_y) return -1;
    int idx = (y - items_y) / item_h;
    if (idx >= 0 && idx < count) return idx;
    return -1;
}

void widget_draw_toggle(int x, int y, int w, int h, int enabled)
{
    uint32_t bg = enabled ? 0x004878B0 : 0x00405060;
    uint32_t knob = enabled ? 0x00FFFFFF : 0x008090A0;
    gfx_fill_rect_rounded(x, y, w, h, h/2, bg);
    int knob_radius = h/2 - 2;
    int knob_x = enabled ? x + w - h + 2 : x + 2;
    gfx_fill_circle(knob_x + knob_radius, y + knob_radius + 1, knob_radius - 1, knob);
}

void widget_draw_progress(int x, int y, int w, int h, int percent)
{
    gfx_fill_rect_rounded(x, y, w, h, h/2, 0x0020304C);
    if (percent > 0) {
        int fill_w = (w * percent) / 100;
        if (fill_w < 4) fill_w = 4;
        gfx_fill_rect_rounded(x, y, fill_w, h, h/2, 0x004878B0);
    }
}
