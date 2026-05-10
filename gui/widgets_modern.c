#include "widgets_modern.h"
#include "gfx_backend.h"
#include "gfx_utils.h"
#include "font_engine.h"
#include <stdlib.h>
#include <string.h>

void widgets_modern_init(void)
{
    theme_init();
}

modern_button_t *button_create(int x, int y, int w, int h, const char *label)
{
    /* Static allocation - no malloc in kernel */
    static modern_button_t btn;
    
    btn.x = x;
    btn.y = y;
    btn.width = w;
    btn.height = h;
    btn.label = label;
    btn.state = UI_STATE_NORMAL;
    btn.on_click = NULL;
    btn.internal_state.hovered = 0;
    btn.internal_state.active = 0;
    btn.internal_state.focused = 0;
    btn.internal_state.disabled = 0;
    
    return &btn;
}

int button_is_hovered(modern_button_t *btn, int mouse_x, int mouse_y)
{
    return (mouse_x >= btn->x && mouse_x < btn->x + btn->width &&
            mouse_y >= btn->y && mouse_y < btn->y + btn->height);
}

void button_handle_mouse_over(modern_button_t *btn, int mouse_x, int mouse_y)
{
    int hovered = button_is_hovered(btn, mouse_x, mouse_y);
    btn->internal_state.hovered = hovered;
}

void button_handle_click(modern_button_t *btn, int mouse_x, int mouse_y)
{
    if (button_is_hovered(btn, mouse_x, mouse_y)) {
        if (btn->on_click) {
            btn->on_click();
        }
    }
}

void button_render(modern_button_t *btn)
{
    theme_t *theme = theme_get();
    
    /* Determine button color based on state */
    uint32_t bg_color = theme_get_button_color(btn->state);
    uint32_t text_color = theme_get_text_color(btn->state);
    
    /* Draw button background with rounded corners */
    gfx_fill_rect_rounded(btn->x, btn->y, btn->width, btn->height, 
                          theme->spacing.radius, bg_color);
    
    /* Draw button border */
    if (btn->internal_state.focused) {
        gfx_stroke_rect_rounded(btn->x, btn->y, btn->width, btn->height,
                               theme->spacing.radius, 2, theme->colors.accent);
    } else {
        gfx_stroke_rect_rounded(btn->x, btn->y, btn->width, btn->height,
                               theme->spacing.radius, 1, theme->colors.border_light);
    }
    
    /* Draw label text (centered) */
    int text_width = 8 * strlen(btn->label); /* Approximate */
    int text_x = btn->x + (btn->width - text_width) / 2;
    int text_y = btn->y + (btn->height - 12) / 2;
    
    font_draw_text(text_x, text_y, btn->label, text_color, 0);
}

void widget_update_state(widget_state_t *state, int hovered, int active)
{
    state->hovered = hovered;
    state->active = active;
}

ui_state_t widget_get_ui_state(widget_state_t *state)
{
    if (state->disabled) return UI_STATE_DISABLED;
    if (state->focused) return UI_STATE_FOCUSED;
    if (state->active) return UI_STATE_ACTIVE;
    if (state->hovered) return UI_STATE_HOVER;
    return UI_STATE_NORMAL;
}
