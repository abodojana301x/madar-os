#ifndef _PS2_H
#define _PS2_H

#include "../include/types.h"
#include "../include/kprintf.h"

/* PS/2 Controller Ports */
#define PS2_DATA_PORT       0x60
#define PS2_COMMAND_PORT    0x64
#define PS2_STATUS_PORT     0x64

/* PS/2 Status Register Bits */
#define PS2_STATUS_OUTPUT_FULL  0x01  /* Output buffer full */
#define PS2_STATUS_INPUT_FULL   0x02  /* Input buffer full */
#define PS2_STATUS_SYSTEM_FLAG  0x04  /* System flag */
#define PS2_STATUS_CMD_DATA     0x08  /* Command/data (1=cmd, 0=data) */
#define PS2_STATUS_TIMEOUT      0x40  /* Timeout error */
#define PS2_STATUS_PARITY       0x80  /* Parity error */

/* PS/2 Controller Commands */
#define PS2_CMD_READ_CONFIG     0x20
#define PS2_CMD_WRITE_CONFIG    0x60
#define PS2_CMD_DISABLE_PORT1   0xAD
#define PS2_CMD_DISABLE_PORT2   0xA7
#define PS2_CMD_ENABLE_PORT1    0xAE
#define PS2_CMD_ENABLE_PORT2    0xA8
#define PS2_CMD_SELF_TEST       0xAA
#define PS2_CMD_PORT1_TEST      0xAB
#define PS2_CMD_PORT2_TEST      0xA9
#define PS2_CMD_CPU_RESET       0xFE

/* PS/2 Device Commands */
#define PS2_DEVICE_RESET       0xFF
#define PS2_DEVICE_RESEND      0xFE
#define PS2_DEVICE_IDENTIFY    0xF2
#define PS2_DEVICE_SET_SAMPLE  0xF3
#define PS2_DEVICE_ENABLE      0xF4
#define PS2_DEVICE_DISABLE     0xF5
#define PS2_DEVICE_SET_DEFAULTS 0xF6

/* PS/2 Keyboard Responses */
#define PS2_RESPONSE_ACK       0xFA
#define PS2_RESPONSE_RESEND    0xFE
#define PS2_RESPONSE_ERROR     0xFF

/* Ring Buffer for Keyboard Input */
#define PS2_BUFFER_SIZE 256

typedef struct {
    uint8_t buffer[PS2_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} ring_buffer_t;

/* Function Prototypes */
void ps2_init(void);
void ps2_wait_write(void);
void ps2_wait_read(void);
uint8_t ps2_read(void);
void ps2_write(uint8_t data);
void ps2_write_command(uint8_t cmd);
void ps2_device_write(uint8_t device, uint8_t cmd);
uint8_t ps2_device_read(uint8_t device);
void ps2_handler_keyboard(void);
void ps2_handler_mouse(void);

/* Keyboard Functions */
void kbd_init(void);
uint8_t kbd_get_char(void);
uint32_t kbd_has_input(void);

/* Mouse Functions */
void mouse_init(void);
typedef struct {
    int8_t x_delta;
    int8_t y_delta;
    uint8_t buttons;
} mouse_packet_t;
mouse_packet_t mouse_get_packet(void);

#endif /* _PS2_H */
