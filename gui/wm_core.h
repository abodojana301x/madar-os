#ifndef GUI_WM_CORE_H
#define GUI_WM_CORE_H

#include <types.h>

#define WM_MAX_WINDOWS 64
#define WM_TITLE_MAX   64

typedef enum {
    WM_NORMAL = 0,
    WM_MINIMIZED = 1,
    WM_MAXIMIZED = 2
} wm_state_t;

typedef struct {
    int used;
    uint32_t id;
    int x;
    int y;
    int w;
    int h;
    int z_index;
    wm_state_t state;
    uint8_t opacity;
    uint8_t is_glass;
    int corner_radius;
    char title[WM_TITLE_MAX];
} wm_window_t;

void wm_init(void);
int wm_create(const char *title, int x, int y, int w, int h);
int wm_create_glass(const char *title, int x, int y, int w, int h);
int wm_move(uint32_t id, int x, int y);
int wm_resize(uint32_t id, int w, int h);
int wm_minimize(uint32_t id);
int wm_maximize(uint32_t id);
int wm_close(uint32_t id);
int wm_raise(uint32_t id);
int wm_window_at(int x, int y, uint32_t *id_out);
int wm_window_at_titlebar(int x, int y, uint32_t *id_out);
int wm_get_count(void);
void wm_draw_all(void);
wm_window_t *wm_get_by_id(uint32_t id);

int wm_handle_click(int x, int y, uint32_t *focus_id);
int wm_handle_mouse_down(int x, int y, uint32_t *window_id);
int wm_handle_mouse_move(int x, int y);
int wm_handle_mouse_up(void);
int wm_handle_mouse_right_down(int x, int y);
int wm_handle_mouse_right_up(int x, int y);
int wm_check_button_hit(int x, int y, uint32_t win_id, int *button_type);

#endif
