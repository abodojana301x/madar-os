#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include <types.h>

#define WIDGET_MAX_ITEMS 32

typedef enum {
    WIDGET_BUTTON = 0,
    WIDGET_TEXTFIELD = 1,
    WIDGET_LIST = 2,
    WIDGET_MENU = 3,
    WIDGET_DIALOG = 4
} widget_type_t;

void widgets_init(void);
void widget_draw_button(int x, int y, int w, int h, const char *label, int hover);
void widget_draw_button_modern(int x, int y, int w, int h, const char *label,
                               uint32_t bg_color, uint32_t text_color, int radius);
void widget_draw_textfield(int x, int y, int w, int h, const char *text, int focused);
int widget_hit_test(int x, int y, int wx, int wy, int ww, int wh);
void widget_draw_list(int x, int y, int w, int h, const char **items, int count, int selected);
void widget_draw_menu(int x, int y, int w, int h, const char **items, int count, int selected);
void widget_draw_dialog(int x, int y, int w, int h, const char *title, const char *message);
int widget_list_hittest(int x, int y, int items_y, int item_h, int count);
void widget_draw_toggle(int x, int y, int w, int h, int enabled);
void widget_draw_progress(int x, int y, int w, int h, int percent);

#endif
