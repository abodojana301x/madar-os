#include "input_poller.h"
#include "event_loop.h"
#include "gfx_backend.h"
#include "../drivers/kbd.h"
#include "../drivers/mouse.h"

static int prev_left_btn = 0;
static int prev_right_btn = 0;

void input_poller_init(void)
{
    prev_left_btn = 0;
    prev_right_btn = 0;
}

void input_poller_poll(void)
{
    gui_event_t ev = {0};

    kbd_poll();
    mouse_poll();

    while (kbd_has_data()) {
        uint8_t scancode = kbd_read_scancode();
        kbd_modifier_t mods = kbd_get_modifiers();

        ev.type = GUI_EVENT_KEY;
        ev.key = scancode;
        ev.ascii = 0;
        ev.modifiers = 0;
        if (mods.lshift || mods.rshift) ev.modifiers |= 1;
        if (mods.lctrl || mods.rctrl) ev.modifiers |= 2;
        if (mods.lalt || mods.ralt) ev.modifiers |= 4;

        event_loop_inject(&ev);
    }

    if (1) {
        static int prev_x = 0, prev_y = 0;

        if (mouse_state.x != prev_x || mouse_state.y != prev_y) {
            int max_x = (int)gfx_get_width() - 1;
            int max_y = (int)gfx_get_height() - 1;
            if (mouse_state.x < 0) mouse_state.x = 0;
            if (mouse_state.y < 0) mouse_state.y = 0;
            if (mouse_state.x > max_x) mouse_state.x = max_x;
            if (mouse_state.y > max_y) mouse_state.y = max_y;

            ev.type = GUI_EVENT_MOUSE_MOVE;
            ev.x = mouse_state.x;
            ev.y = mouse_state.y;
            event_loop_inject(&ev);

            prev_x = mouse_state.x;
            prev_y = mouse_state.y;
        }

        if (mouse_state.left_btn && !prev_left_btn) {
            ev.type = GUI_EVENT_MOUSE_DOWN;
            ev.x = mouse_state.x;
            ev.y = mouse_state.y;
            event_loop_inject(&ev);
        }

        if (!mouse_state.left_btn && prev_left_btn) {
            ev.type = GUI_EVENT_MOUSE_UP;
            ev.x = mouse_state.x;
            ev.y = mouse_state.y;
            event_loop_inject(&ev);
        }

        if (mouse_state.right_btn && !prev_right_btn) {
            ev.type = GUI_EVENT_MOUSE_RIGHT_DOWN;
            ev.x = mouse_state.x;
            ev.y = mouse_state.y;
            event_loop_inject(&ev);
        }

        if (!mouse_state.right_btn && prev_right_btn) {
            ev.type = GUI_EVENT_MOUSE_RIGHT_UP;
            ev.x = mouse_state.x;
            ev.y = mouse_state.y;
            event_loop_inject(&ev);
        }

        prev_left_btn = mouse_state.left_btn;
        prev_right_btn = mouse_state.right_btn;
    }
}
