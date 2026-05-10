#include "desktop.h"
#include "../gui/gfx_backend.h"
#include "../gui/gfx_utils.h"
#include "../gui/font_engine.h"
#include "../gui/wm_core.h"
#include <kprintf.h>

int desktop_shell_init(void)
{
    if (desktop_session_init() != 0) return -1;
    if (desktop_app_registry_init() != 0) return -1;
    if (desktop_start_menu_init() != 0) return -1;
    if (desktop_panel_init() != 0) return -1;

    kprintf("Madar Desktop Shell v0.2 initialized (%ux%u)\n", gfx_get_width(), gfx_get_height());
    return 0;
}

void desktop_shell_draw_desktop(void)
{
    uint32_t w = gfx_get_width();
    uint32_t h = gfx_get_height();
    gfx_clear(0x00101825);
    for (uint32_t y = 0; y < h; y++) {
        uint32_t c;
        if (y < h / 4) {
            c = 0x000C1831;
        } else if (y < h / 2) {
            c = 0x00101F3A;
        } else if (y < (3 * h) / 4) {
            c = 0x00142646;
        } else {
            c = 0x000D1830;
        }
        gfx_fill_rect(0, (int)y, (int)w, 1, c);
    }

    gfx_fill_circle((int)((w * 18) / 100), (int)((h * 16) / 100), (int)(w / 6), 0x00112346);
    gfx_fill_circle((int)((w * 78) / 100), (int)((h * 18) / 100), (int)(w / 5), 0x00142F66);
    gfx_fill_circle((int)((w * 68) / 100), (int)((h * 72) / 100), (int)(w / 4), 0x000E2148);
    gfx_fill_circle((int)((w * 38) / 100), (int)((h * 82) / 100), (int)(w / 4), 0x00152349);

    gfx_draw_shadow((int)w - 320, 22, 280, 68, 16, 0x00000000, 64);
    gfx_fill_rect_rounded_blend((int)w - 318, 24, 276, 64, 20, 0xA0374F7B);
    gfx_fill_rect_rounded_blend((int)w - 316, 26, 272, 60, 18, 0xA0466395);
    gfx_fill_rect_rounded_blend((int)w - 314, 28, 268, 14, 16, 0xA0738FBD);
    font_draw_text((int)w - 280, 46, "MADAR OS", 0x00F7FBFF, 0);
    font_draw_text((int)w - 280, 64, "Fluent glass shell", 0x00D5E6FF, 0);
}

void desktop_shell_draw(void)
{
    desktop_start_menu_draw();
    desktop_panel_draw();
}

int desktop_shell_handle_click(int x, int y)
{
    if (desktop_start_menu_handle_click(x, y) == 0) return 0;
    if (desktop_panel_handle_click(x, y) == 0) return 0;
    desktop_start_menu_close();
    return -1;
}

int desktop_shell_selftest(void)
{
    kprintf("Testing Madar Desktop Shell...\n");
    if (desktop_app_count() < 4) return -1;
    if (!desktop_app_find("terminal")) return -1;
    if (!desktop_app_find("files")) return -1;
    if (!desktop_app_find("settings")) return -1;
    if (!desktop_app_find("tasks")) return -1;
    desktop_start_menu_toggle();
    if (!desktop_start_menu_is_open()) return -1;
    desktop_start_menu_close();
    if (desktop_start_menu_is_open()) return -1;
    kprintf("Madar Desktop Shell PASSED\n");
    return 0;
}
