#ifndef MOUSE_H
#define MOUSE_H

#include <types.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64
#define PS2_STATUS_OUTPUT_FULL 0x01
#define PS2_STATUS_INPUT_FULL 0x02
#define PS2_STATUS_AUX_DATA 0x20
#define PS2_CMD_ENABLE_AUX 0xA8
#define PS2_CMD_READ_CONFIG 0x20
#define PS2_CMD_WRITE_CONFIG 0x60
#define PS2_CMD_WRITE_MOUSE 0xD4
#define PS2_DEV_SET_DEFAULTS 0xF6
#define PS2_DEV_ENABLE_STREAMING 0xF4
#define PS2_ACK 0xFA

typedef struct {
    int8_t x_delta;
    int8_t y_delta;
    int8_t z_delta;
    uint8_t left_btn;
    uint8_t right_btn;
    uint8_t middle_btn;
} mouse_packet_t;

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t left_btn;
    uint8_t right_btn;
    uint8_t middle_btn;
} mouse_state_t;

extern mouse_state_t mouse_state;

void mouse_init(void);
void mouse_poll(void);
void mouse_handler(void);
mouse_packet_t mouse_get_packet(void);
int mouse_is_packet_ready(void);

#endif
