#include "keyboard_system.h"
#include <string.h>

static keyboard_state_t g_keyboard = {0};

void keyboard_system_init(void)
{
    memset(&g_keyboard, 0, sizeof(keyboard_state_t));
}

void keyboard_handle_key_down(int key)
{
    if (key >= 0 && key < MAX_KEYS) {
        g_keyboard.key_states[key] = 1;
    }
}

void keyboard_handle_key_up(int key)
{
    if (key >= 0 && key < MAX_KEYS) {
        g_keyboard.key_states[key] = 0;
    }
}

int keyboard_is_pressed(int key)
{
    if (key < 0 || key >= MAX_KEYS) return 0;
    return g_keyboard.key_states[key] != 0;
}

int keyboard_was_pressed(int key)
{
    if (key < 0 || key >= MAX_KEYS) return 0;
    return g_keyboard.prev_key_states[key] != 0;
}

int keyboard_key_pressed(int key)
{
    if (key < 0 || key >= MAX_KEYS) return 0;
    /* Edge trigger: was not pressed before, is pressed now */
    return !g_keyboard.prev_key_states[key] && g_keyboard.key_states[key];
}

int keyboard_is_alt_held(void)
{
    return g_keyboard.key_states[KEY_ALT] || g_keyboard.key_states[KEY_RALT];
}

int keyboard_is_ctrl_held(void)
{
    return g_keyboard.key_states[KEY_LCTRL] || g_keyboard.key_states[KEY_RCTRL];
}

int keyboard_is_shift_held(void)
{
    return g_keyboard.key_states[KEY_LSHIFT] || g_keyboard.key_states[KEY_RSHIFT];
}

int keyboard_check_alt_tab_pressed(void)
{
    /* Alt+Tab edge trigger: Alt is held AND Tab key just pressed */
    if (keyboard_is_alt_held() && keyboard_key_pressed(KEY_TAB)) {
        return 1;
    }
    return 0;
}

void keyboard_system_frame_end(void)
{
    /* Save current state for next frame's edge detection */
    memcpy(g_keyboard.prev_key_states, g_keyboard.key_states, MAX_KEYS);
}
