#ifndef GUI_KEYBOARD_SYSTEM_H
#define GUI_KEYBOARD_SYSTEM_H

#include <types.h>

/* Keyboard key codes (PS/2 based) */
#define KEY_LSHIFT  0x12
#define KEY_RSHIFT  0x59
#define KEY_LCTRL   0x1D
#define KEY_RCTRL   0x9D
#define KEY_ALT     0x38
#define KEY_RALT    0xB8
#define KEY_TAB     0x0F
#define KEY_ESCAPE  0x01
#define KEY_ENTER   0x1C
#define KEY_SPACE   0x39

/* Key state tracking (256 possible keys) */
#define MAX_KEYS    256

typedef struct {
    uint8_t key_states[MAX_KEYS];    /* 0 = up, 1 = down */
    uint8_t prev_key_states[MAX_KEYS]; /* Previous frame state for edge detection */
    uint32_t last_tab_press;           /* Timestamp of last Alt+Tab */
} keyboard_state_t;

/* Initialize keyboard system */
void keyboard_system_init(void);

/* Handle key down event */
void keyboard_handle_key_down(int key);

/* Handle key up event */
void keyboard_handle_key_up(int key);

/* Get current key state */
int keyboard_is_pressed(int key);

/* Get previous frame key state (for edge detection) */
int keyboard_was_pressed(int key);

/* Detect key press edge (transition from up to down) */
int keyboard_key_pressed(int key);

/* Check if modifier key is held (ALT, CTRL, SHIFT) */
int keyboard_is_alt_held(void);
int keyboard_is_ctrl_held(void);
int keyboard_is_shift_held(void);

/* Check for Alt+Tab combination with edge trigger */
int keyboard_check_alt_tab_pressed(void);

/* Update frame state (call this at end of each frame) */
void keyboard_system_frame_end(void);

#endif
