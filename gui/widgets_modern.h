#ifndef GUI_WIDGETS_MODERN_H
#define GUI_WIDGETS_MODERN_H

#include <types.h>
#include "theme.h"

/* Modern widget state tracking */
typedef struct {
    int hovered;
    int active;
    int focused;
    int disabled;
} widget_state_t;

/* Modern button with state tracking */
typedef struct {
    int x, y;
    int width, height;
    const char *label;
    ui_state_t state;
    int (*on_click)(void);
    widget_state_t internal_state;
} modern_button_t;

/* Setup and initialization */
void widgets_modern_init(void);

/* Button functions */
modern_button_t *button_create(int x, int y, int w, int h, const char *label);
void button_render(modern_button_t *btn);
void button_handle_mouse_over(modern_button_t *btn, int mouse_x, int mouse_y);
void button_handle_click(modern_button_t *btn, int mouse_x, int mouse_y);
int button_is_hovered(modern_button_t *btn, int mouse_x, int mouse_y);

/* State machine */
void widget_update_state(widget_state_t *state, int hovered, int active);
ui_state_t widget_get_ui_state(widget_state_t *state);

#endif
