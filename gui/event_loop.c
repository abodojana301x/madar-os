#include "event_loop.h"
#include "wm_core.h"
#include "gfx_backend.h"
#include "taskbar.h"
#include "keyboard_system.h"
#include "input_system.h"
#include "../desktop/desktop.h"
#include "../drivers/mouse.h"
#include <timer.h>

extern void input_router_init(void);
extern int input_router_push(const gui_event_t *ev);
extern int input_router_pop(gui_event_t *ev);
extern void focus_set(uint32_t id);

static uint64_t g_last_latency = 0;

static void draw_cursor(int x, int y)
{
    static const char *cursor_shape[] = {
        "1000000000000000",
        "1100000000000000",
        "1110000000000000",
        "1111000000000000",
        "1111100000000000",
        "1111110000000000",
        "1111111000000000",
        "1111111100000000",
        "1111111110000000",
        "1111110111000000",
        "1111000011100000",
        "1110000001110000",
        "1100000000111000",
        "1000000000011100",
        "0000000000001110",
        "0000000000000111",
        "0000001100000011",
        "0000011110000001",
        "0000011111000000",
        "0000001110000000"
    };

    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 16; col++) {
            if (cursor_shape[row][col] != '1') continue;
            gfx_draw_pixel(x + col, y + row, 0x00000000);
            if (col > 0) gfx_draw_pixel(x + col - 1, y + row, 0x00FFFFFF);
            if (row > 0) gfx_draw_pixel(x + col, y + row - 1, 0x00FFFFFF);
        }
    }
}

void event_loop_init(void)
{
    input_router_init();
    keyboard_system_init();
    input_system_init();
    g_last_latency = 0;
}

void event_loop_inject(const gui_event_t *ev)
{
    (void)input_router_push(ev);
}

uint32_t event_loop_latency_ms(void)
{
    return (uint32_t)g_last_latency;
}

int event_loop_step(void)
{
    uint64_t start = timer_get_ticks();
    gui_event_t ev;
    int handled = 0;

    for (int n = 0; n < 50; n++) {
        if (input_router_pop(&ev) != 0) break;
        handled++;

        switch (ev.type) {
        case GUI_EVENT_MOUSE_DOWN:
            /* Handle window manager mouse down (drag start) */
            {
                uint32_t wid = 0;
                if (desktop_shell_handle_click(ev.x, ev.y) == 0) {
                    /* Desktop shell handled interaction. */
                } else if (taskbar_click(ev.x, ev.y, &wid) == 0) {
                    (void)wm_raise(wid);
                    focus_set(wid);
                } else {
                    wm_handle_mouse_down(ev.x, ev.y, &wid);
                    if (wid != 0) {
                        focus_set(wid);
                    }
                }
            }
            break;

        case GUI_EVENT_MOUSE_UP:
            /* Handle mouse up (drag end) */
            (void)wm_handle_mouse_up();
            break;

        case GUI_EVENT_MOUSE_MOVE:
            /* Handle window dragging and hover detection */
            (void)wm_handle_mouse_move(ev.x, ev.y);
            input_system_update_hover(ev.x, ev.y);
            break;

        case GUI_EVENT_MOUSE_CLICK:
            /* Legacy mouse click handling - treat as down for now */
            {
                uint32_t wid = 0;
                if (desktop_shell_handle_click(ev.x, ev.y) == 0) {
                    /* Desktop shell handled launcher/menu/panel interaction. */
                } else if (taskbar_click(ev.x, ev.y, &wid) == 0) {
                    (void)wm_raise(wid);
                    focus_set(wid);
                } else if (wm_window_at(ev.x, ev.y, &wid) == 0) {
                    (void)wm_raise(wid);
                    focus_set(wid);
                }
            }
            break;

        case GUI_EVENT_MOUSE_RIGHT_DOWN:
            (void)wm_handle_mouse_right_down(ev.x, ev.y);
            break;

        case GUI_EVENT_MOUSE_RIGHT_UP:
            (void)wm_handle_mouse_right_up(ev.x, ev.y);
            break;

        case GUI_EVENT_KEY:
            {
                int is_release = (ev.key & 0x80) ? 1 : 0;
                int key = ev.key & 0x7F;
                if (is_release) {
                    keyboard_handle_key_up(key);
                } else {
                    keyboard_handle_key_down(key);
                }
            }
            break;

        default:
            break;
        }
    }

    /* Check for Alt+Tab combination with edge trigger */
    if (keyboard_check_alt_tab_pressed()) {
        input_system_cycle_windows_forward();
    }

    desktop_shell_draw_desktop();
    wm_draw_all();
    desktop_shell_draw();
    draw_cursor(mouse_state.x, mouse_state.y);
    gfx_swap_buffers();

    /* Update frame state for next frame's edge detection */
    keyboard_system_frame_end();

    g_last_latency = timer_get_ticks() - start;
    return handled;
}
