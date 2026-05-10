#ifndef KBD_H
#define KBD_H

#include <types.h>

#define PS2_DATA_PORT       0x60
#define PS2_STATUS_PORT     0x64
#define PS2_CMD_PORT        0x64

#define PS2_CMD_READ_CONFIG     0x20
#define PS2_CMD_WRITE_CONFIG    0x60
#define PS2_CMD_TEST_CONTROLLER 0xAA
#define PS2_CMD_TEST_KEYBOARD   0xAB
#define PS2_CMD_ENABLE_KBD      0xAE
#define PS2_CMD_DISABLE_KBD     0xAD

#define KBD_ACK             0xFA
#define KBD_RESEND          0xFE
#define KBD_SELF_TEST_PASS  0xAA

#define SCANCODE_ESCAPE     0x01
#define SCANCODE_BACKSPACE  0x0E
#define SCANCODE_TAB        0x0F
#define SCANCODE_ENTER      0x1C
#define SCANCODE_SPACE      0x39
#define SCANCODE_SHIFT_L    0x2A
#define SCANCODE_SHIFT_R    0x36
#define SCANCODE_CTRL_L     0x1D
#define SCANCODE_ALT_L      0x38
#define SCANCODE_CAPS_LOCK  0x3A
#define SCANCODE_NUM_LOCK   0x45
#define SCANCODE_DELETE     0x53

#define KBD_BUFFER_SIZE     256

typedef struct {
    uint8_t lctrl : 1;
    uint8_t rctrl : 1;
    uint8_t lshift : 1;
    uint8_t rshift : 1;
    uint8_t lalt : 1;
    uint8_t ralt : 1;
    uint8_t caps : 1;
    uint8_t num : 1;
} kbd_modifier_t;

typedef struct {
    uint8_t buffer[KBD_BUFFER_SIZE];
    uint8_t buffer_ext[KBD_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t buffer_size;
    uint8_t extended;
    uint8_t trace;
} kbd_buffer_t;

void kbd_init(void);
void kbd_poll(void);
uint8_t kbd_read_scancode(void);
uint8_t kbd_read_ascii(void);
uint8_t kbd_has_data(void);
int kbd_is_scancode_release(void);
kbd_modifier_t kbd_get_modifiers(void);
void kbd_set_trace(int enabled);
void kbd_handler(void);

#endif
