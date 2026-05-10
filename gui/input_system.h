#ifndef GUI_INPUT_SYSTEM_H
#define GUI_INPUT_SYSTEM_H

#include <types.h>

/* Hover state tracking */
typedef struct {
    uint32_t hovered_window_id;  /* Window ID currently hovered over */
    int hover_x;                  /* Hover position X */
    int hover_y;                  /* Hover position Y */
} hover_state_t;

/* Initialize input system (mouse + keyboard) */
void input_system_init(void);

/* Update hover state based on current mouse position */
void input_system_update_hover(int x, int y);

/* Get hovered window ID */
uint32_t input_system_get_hovered_window(void);

/* Cycle through windows (Alt+Tab functionality) */
int input_system_cycle_windows_forward(void);

/* Cycle through windows in reverse order */
int input_system_cycle_windows_backward(void);

/* Get count of visible windows */
int input_system_get_window_count(void);

#endif
