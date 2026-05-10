#include "ui_toolkit.h"
#include "gfx_backend.h"
#include "font_engine.h"
#include <stdlib.h>
#include <string.h>
#include "heap.h"

/* Wrapper for malloc to use kernel allocator */
#define malloc(size) kmalloc(size)

/* ============ CORE RENDERING ============ */

void ui_render_element(UIElement* el) {
    if (!el || !el->visible) return;

    if (el->render) {
        el->render(el);
    }
}

void ui_render_container(UIContainer* c) {
    if (!c || !ui_is_visible((UIElement*)c)) return;

    /* Render container background */
    ui_render_element((UIElement*)c);

    /* Render all children in order */
    for (int i = 0; i < c->child_count; i++) {
        ui_render_element(c->children[i]);
    }
}

/* ============ CONTAINER MANAGEMENT ============ */

void ui_container_add_child(UIContainer* c, UIElement* child) {
    if (!c || !child || c->child_count >= UI_MAX_CHILDREN) return;

    c->children[c->child_count] = child;
    c->child_count++;
    child->parent = (UIElement*)c;
}

void ui_container_clear(UIContainer* c) {
    if (!c) return;
    c->child_count = 0;
}

/* ============ LAYOUT SYSTEM ============ */

void ui_layout_vertical(UIContainer* c, int start_x, int start_y, int spacing) {
    if (!c) return;

    int offset = 0;

    for (int i = 0; i < c->child_count; i++) {
        UIElement* child = c->children[i];

        if (ui_is_visible(child)) {
            child->y = start_y + offset;
            child->x = start_x;

            offset += child->height + spacing;
        }
    }
}

void ui_layout_horizontal(UIContainer* c, int start_x, int start_y, int spacing) {
    if (!c) return;

    int offset = 0;

    for (int i = 0; i < c->child_count; i++) {
        UIElement* child = c->children[i];

        if (ui_is_visible(child)) {
            child->x = start_x + offset;
            child->y = start_y;

            offset += child->width + spacing;
        }
    }
}

/* ============ WIDGET RENDERING ============ */

void ui_render_button(UIElement* el) {
    Button* btn = (Button*)el;

    uint32_t color = btn->is_hovered ? COLOR_BTN_HOVER : COLOR_BTN;

    gfx_fill_rect(el->x, el->y, el->width, el->height, color);

    if (btn->text) {
        /* Center text approximately */
        int text_x = el->x + (el->width - font_get_text_width(btn->text)) / 2;
        int text_y = el->y + (el->height - 8) / 2;
        font_draw_text(text_x, text_y, btn->text, COLOR_BTN_TEXT, 0);
    }
}

void ui_render_label(UIElement* el) {
    Label* lbl = (Label*)el;

    if (lbl->text) {
        font_draw_text(el->x, el->y, lbl->text, lbl->color, 0);
    }
}

void ui_render_container_bg(UIElement* el) {
    (void)el;  /* Not used in current implementation */
    /* Background rendering handled by gfx_backend */
}

/* ============ HOVER DETECTION ============ */

static Button* g_hovered_button = NULL;

void ui_update_hover(UIElement* root, int mouse_x, int mouse_y) {
    if (!root) return;

    /* Reset previous hover state */
    if (g_hovered_button) {
        g_hovered_button->is_hovered = 0;
        g_hovered_button = NULL;
    }

    /* Find element under mouse by traversing all visible containers */
    if (root->type == UI_TYPE_CONTAINER) {
        UIContainer* c = (UIContainer*)root;

        for (int i = c->child_count - 1; i >= 0; i--) {
            UIElement* child = c->children[i];

            if (ui_is_visible(child) &&
                mouse_x >= child->x && mouse_x <= child->x + child->width &&
                mouse_y >= child->y && mouse_y <= child->y + child->height) {

                if (child->type == UI_TYPE_BUTTON) {
                    Button* btn = (Button*)child;
                    btn->is_hovered = 1;
                    g_hovered_button = btn;
                }

                if (child->on_hover) {
                    child->on_hover(child, 1);
                }

                break;
            }
        }
    }
}

/* ============ EVENT HANDLING ============ */

UIElement* ui_find_clicked(UIElement* root, int x, int y) {
    if (!root) return NULL;

    /* Check children in reverse order (top-most first) */
    if (root->type == UI_TYPE_CONTAINER) {
        UIContainer* c = (UIContainer*)root;

        for (int i = c->child_count - 1; i >= 0; i--) {
            UIElement* child = c->children[i];

            if (ui_is_visible(child) &&
                x >= child->x && x <= child->x + child->width &&
                y >= child->y && y <= child->y + child->height) {

                return child;
            }
        }
    }

    /* Check root itself (if visible and within bounds) */
    if (ui_is_visible(root) &&
        x >= root->x && x <= root->x + root->width &&
        y >= root->y && y <= root->y + root->height) {
        return root;
    }

    return NULL;
}

void ui_handle_click(UIElement* root, int x, int y) {
    UIElement* el = ui_find_clicked(root, x, y);

    if (el && el->on_click) {
        el->on_click(el);
    }
}

/* ============ WIDGET CREATION ============ */

Button* ui_button_create(int x, int y, int w, int h, const char* text) {
    Button* btn = (Button*)malloc(sizeof(Button));

    if (!btn) return NULL;

    btn->base.type = UI_TYPE_BUTTON;
    btn->base.x = x;
    btn->base.y = y;
    btn->base.width = w;
    btn->base.height = h;
    btn->base.visible = 1;
    btn->base.render = ui_render_button;
    btn->base.on_click = NULL;
    btn->base.on_hover = NULL;
    btn->base.parent = NULL;

    /* Copy text */
    btn->text = (char*)malloc(strlen(text) + 1);
    if (btn->text) {
        strncpy(btn->text, text, strlen(text) + 1);
    }

    btn->is_hovered = 0;

    return btn;
}

Label* ui_label_create(int x, int y, const char* text) {
    Label* lbl = (Label*)malloc(sizeof(Label));

    if (!lbl) return NULL;

    lbl->base.type = UI_TYPE_LABEL;
    lbl->base.x = x;
    lbl->base.y = y;
    lbl->base.width = font_get_text_width(text);
    lbl->base.height = 16; /* Approximate font height */
    lbl->base.visible = 1;
    lbl->base.render = ui_render_label;
    lbl->base.on_click = NULL;
    lbl->base.on_hover = NULL;
    lbl->base.parent = NULL;

    /* Copy text */
    lbl->text = (char*)malloc(strlen(text) + 1);
    if (lbl->text) {
        strncpy(lbl->text, text, strlen(text) + 1);
    }

    lbl->color = COLOR_LABEL;

    return lbl;
}

UIContainer* ui_container_create(int x, int y, int w, int h) {
    UIContainer* c = (UIContainer*)malloc(sizeof(UIContainer));

    if (!c) return NULL;

    c->base.type = UI_TYPE_CONTAINER;
    c->base.x = x;
    c->base.y = y;
    c->base.width = w;
    c->base.height = h;
    c->base.visible = 1;
    c->base.render = ui_render_container_bg;
    c->base.on_click = NULL;
    c->base.on_hover = NULL;
    c->base.parent = NULL;

    c->child_count = 0;

    return c;
}

/* ============ UTILITY ============ */

void ui_set_visible(UIElement* el, int visible) {
    if (el) {
        el->visible = visible;
    }
}

int ui_is_visible(UIElement* el) {
    return el && el->visible;
}

void ui_set_position(UIElement* el, int x, int y) {
    if (el) {
        el->x = x;
        el->y = y;
    }
}

void ui_set_size(UIElement* el, int w, int h) {
    if (el) {
        el->width = w;
        el->height = h;
    }
}

/* ============ INITIALIZATION ============ */

void ui_toolkit_init(void) {
    /* Initialize font engine for text rendering */
    font_init();
    g_hovered_button = NULL;
}
