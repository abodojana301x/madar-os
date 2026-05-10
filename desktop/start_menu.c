#include "desktop.h"
#include "../gui/gfx_backend.h"
#include "../gui/gfx_utils.h"
#include "../gui/font_engine.h"
#include "../gui/widgets.h"

#define MENU_W 400
#define ROW_H 36
#define PANEL_H 48

static int g_open;

int desktop_start_menu_init(void)
{
    g_open = 0;
    return 0;
}

void desktop_start_menu_toggle(void)
{
    g_open = !g_open;
}

void desktop_start_menu_close(void)
{
    g_open = 0;
}

int desktop_start_menu_is_open(void)
{
    return g_open;
}

static int menu_y(void)
{
    int rows = desktop_app_count();
    return (int)gfx_get_height() - PANEL_H - 80 - (rows * ROW_H);
}

static int menu_x(void)
{
    return 18;
}

void desktop_start_menu_draw(void)
{
    if (!g_open) return;
    int rows = desktop_app_count();
    int x = menu_x();
    int y = menu_y();
    int h = 110 + (rows * ROW_H);

    gfx_draw_shadow(x - 8, y - 10, MENU_W + 16, h + 20, 20, 0x00000000, 80);
    gfx_fill_rect_rounded(x, y, MENU_W, h, 20, 0xF0EAF0F8);
    gfx_fill_rect_rounded(x + 3, y + 3, MENU_W - 6, h - 6, 18, 0xF0E0E8F0);

    gfx_fill_rect_rounded(x + 16, y + 12, MENU_W - 32, 28, 10, 0xD0D0DFEC);
    gfx_fill_rect_rounded(x + 18, y + 14, MENU_W - 36, 24, 8, 0xE0E5EDF5);
    font_draw_text(x + 28, y + 20, "Search apps, settings, docs", 0x00607088, 0);

    font_draw_text(x + 20, y + 52, "Pinned", 0x00304058, 0);

    int app_start_y = y + 72;
    for (int i = 0; i < rows; i++) {
        const desktop_app_t *app = desktop_app_get(i);
        if (!app) continue;
        int row_y = app_start_y + (i * ROW_H);

        gfx_fill_rect_rounded_blend(x + 16, row_y, MENU_W - 32, ROW_H - 4, 8, 0x30000000);
        gfx_stroke_rect_rounded(x + 16, row_y, MENU_W - 32, ROW_H - 4, 8, 1, 0x10B0C0D0);

        int icon_size = 20;
        gfx_fill_rect_rounded(x + 22, row_y + 4, icon_size, icon_size, 6, 0x40486888);
        font_draw_text(x + 52, row_y + 8, app->label, 0x00203048, 0);
    }

    gfx_fill_rect_rounded(x + 16, y + h - 24, MENU_W - 32, 18, 8, 0x20B0C0D0);
    font_draw_text_aligned(x + 16, y + h - 22, MENU_W - 32, "Madar OS - All apps", 0x00708098, FONT_ALIGN_CENTER);
}

int desktop_start_menu_handle_click(int x, int y)
{
    if (!g_open) return -1;
    int rows = desktop_app_count();
    int left = menu_x();
    int top = menu_y();
    int bottom = top + 110 + rows * ROW_H;

    if (x < left || x >= left + MENU_W || y < top || y >= bottom) {
        desktop_start_menu_close();
        return -1;
    }

    int app_start_y = top + 72;
    for (int i = 0; i < rows; i++) {
        int row_y = app_start_y + (i * ROW_H);
        if (y >= row_y && y < row_y + ROW_H - 4) {
            const desktop_app_t *app = desktop_app_get(i);
            if (!app) return -1;
            desktop_start_menu_close();
            return desktop_app_launch(app->id) < 0 ? -1 : 0;
        }
    }
    return 0;
}
