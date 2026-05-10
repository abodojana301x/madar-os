#ifndef GUI_UI_TOOLKIT_H
#define GUI_UI_TOOLKIT_H

#include <types.h>
#include "gfx_backend.h"
#include "font_engine.h"

#define UI_MAX_CHILDREN 10

/* Color definitions */
#define COLOR_BTN        0x00506070
#define COLOR_BTN_HOVER  0x007080A0
#define COLOR_BTN_TEXT   0x00FFFFFF
#define COLOR_LABEL      0x00CCCCCC
#define COLOR_CONTAINER  0x00202030

/* Widget types */
typedef enum {
    UI_TYPE_ELEMENT = 0,
    UI_TYPE_CONTAINER,
    UI_TYPE_BUTTON,
    UI_TYPE_LABEL
} ui_type_t;

/* Base UI element */
typedef struct UIElement {
    int x, y;
    int width, height;
    int visible;
    ui_type_t type;

    void (*render)(struct UIElement*);
    void (*on_click)(struct UIElement*);
    void (*on_hover)(struct UIElement*, int hovered);

    struct UIElement* parent;
} UIElement;

/* Container widget - can hold children */
typedef struct {
    UIElement base;

    UIElement* children[UI_MAX_CHILDREN];
    int child_count;
} UIContainer;

/* Button widget */
typedef struct {
    UIElement base;

    char* text;
    int is_hovered;
} Button;

/* Label widget */
typedef struct {
    UIElement base;

    char* text;
    uint32_t color;
} Label;

/* Function prototypes */
void ui_toolkit_init(void);

/* Rendering */
void ui_render_element(UIElement* el);
void ui_render_container(UIContainer* c);

/* Layout */
void ui_layout_vertical(UIContainer* c, int start_x, int start_y, int spacing);
void ui_layout_horizontal(UIContainer* c, int start_x, int start_y, int spacing);

/* Container management */
void ui_container_add_child(UIContainer* c, UIElement* child);
void ui_container_clear(UIContainer* c);

/* Widget creation */
Button* ui_button_create(int x, int y, int w, int h, const char* text);
Label* ui_label_create(int x, int y, const char* text);
UIContainer* ui_container_create(int x, int y, int w, int h);

/* Hover detection */
void ui_update_hover(UIElement* root, int mouse_x, int mouse_y);

/* Event handling */
UIElement* ui_find_clicked(UIElement* root, int x, int y);
void ui_handle_click(UIElement* root, int x, int y);

/* Utility */
void ui_set_visible(UIElement* el, int visible);
int ui_is_visible(UIElement* el);
void ui_set_position(UIElement* el, int x, int y);
void ui_set_size(UIElement* el, int w, int h);

#endif
