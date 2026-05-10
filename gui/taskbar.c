#include "taskbar.h"
#include "gfx_backend.h"
#include "gfx_utils.h"
#include "font_engine.h"
#include "wm_core.h"
#include "theme.h"

#define TASKBAR_H 48
#define ICON_HEIGHT 32
#define ICON_PADDING 8

void taskbar_init(void)
{
    theme_init();
}

void taskbar_draw(void)
{
    uint32_t w = gfx_get_width();
    uint32_t h = gfx_get_height();
    if (h < TASKBAR_H) return;

    theme_t *theme = theme_get();
    
    /* Windows 11 style taskbar background - slightly transparent effect */
    gfx_fill_rect(0, (int)(h - TASKBAR_H), (int)w, TASKBAR_H, 
                  theme->colors.taskbar_bg);
    
    /* Subtle top border - not too prominent */
    gfx_fill_rect(0, (int)(h - TASKBAR_H), (int)w, 1, theme->colors.border_light);
    
    /* Render window icons - centered in taskbar */
    int icon_size = 36;
    int icon_spacing = 8;
    int start_x = 12;  /* Start near left with padding */
    int icon_x = start_x;
    int icon_y = (int)(h - TASKBAR_H) + (TASKBAR_H - icon_size) / 2;
    
    int window_count = wm_get_count();
    for (int i = 0; i < window_count && i < 20; i++) {  /* Max 20 icons */
        wm_window_t *win = wm_get_by_id(i + 1);
        if (!win) continue;
        
        /* Icon background with hover/active effect */
        uint32_t icon_bg = theme->colors.taskbar_icon_normal;
        uint32_t icon_text = theme->colors.text_primary;
        
        /* Windows 11: subtle rounded icon background */
        gfx_fill_rect_rounded(icon_x, icon_y, icon_size, icon_size, 6, icon_bg);
        
        /* Draw window label (first 2 characters) */
        if (win->title[0] != '\0') {
            char label[3];
            label[0] = win->title[0];
            label[1] = (win->title[1] != '\0') ? win->title[1] : ' ';
            label[2] = '\0';
            
            /* Center text in icon */
            font_draw_text(icon_x + 8, icon_y + 10, label, icon_text, 0);
        }
        
        icon_x += icon_size + icon_spacing;
    }
    
    /* Draw system time on the right */
    uint32_t time_x = w - 80;
    font_draw_text((int)time_x, icon_y + 10, "12:00", 
                  theme->colors.text_primary, 0);
}

int taskbar_click(int x, int y, uint32_t *window_id)
{
    uint32_t h = gfx_get_height();
    if ((uint32_t)y < h - TASKBAR_H) return -1;

    /* Calculate which icon was clicked */
    int icon_index = (x - ICON_PADDING) / (ICON_HEIGHT + ICON_PADDING);
    
    if (icon_index >= 0 && icon_index < wm_get_count()) {
        wm_window_t *w = wm_get_by_id(icon_index + 1);
        if (w && window_id) {
            *window_id = w->id;
            return 0;
        }
    }
    
    return -1;
}
