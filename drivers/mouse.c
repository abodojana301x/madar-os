#include "mouse.h"
#include "../include/kprintf.h"
#include "../include/io.h"
#include "../include/spinlock.h"

static mouse_state_t mouse_global = {0};
mouse_state_t mouse_state = {0};

static uint8_t mouse_packet_buffer[4] = {0};
static uint8_t mouse_packet_index = 0;
static int mouse_packet_ready = 0;
static spinlock_t mouse_lock;

#define MOUSE_BASE_SPEED 2
#define MOUSE_ACCEL_THRESHOLD 8
#define MOUSE_ACCEL_MULTIPLIER 1

static int apply_mouse_curve(int delta)
{
    int magnitude = delta < 0 ? -delta : delta;
    int scaled = delta * MOUSE_BASE_SPEED;

    if (magnitude >= MOUSE_ACCEL_THRESHOLD) {
        scaled += (delta * MOUSE_ACCEL_MULTIPLIER * (magnitude - MOUSE_ACCEL_THRESHOLD)) / 4;
    }

    return scaled;
}

static void mouse_wait_write(void)
{
    for (int i = 0; i < 100000; i++) {
        if ((inb(PS2_STATUS_PORT) & PS2_STATUS_INPUT_FULL) == 0) return;
    }
}

static void mouse_wait_read(void)
{
    for (int i = 0; i < 100000; i++) {
        if (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL) return;
    }
}

static void mouse_write_command(uint8_t cmd)
{
    mouse_wait_write();
    outb(PS2_COMMAND_PORT, cmd);
}

static void mouse_write_data(uint8_t data)
{
    mouse_wait_write();
    outb(PS2_DATA_PORT, data);
}

static void mouse_write_device(uint8_t data)
{
    mouse_write_command(PS2_CMD_WRITE_MOUSE);
    mouse_write_data(data);
}

static uint8_t mouse_read_data(void)
{
    mouse_wait_read();
    return inb(PS2_DATA_PORT);
}

static void mouse_process_byte(uint8_t data)
{
    if (mouse_packet_index == 0) {
        if (!(data & 0x08)) return;
        mouse_packet_buffer[0] = data;
        mouse_packet_index = 1;
        return;
    }

    mouse_packet_buffer[mouse_packet_index] = data;
    mouse_packet_index++;

    if (mouse_packet_index >= 3) {
        mouse_packet_index = 0;
        mouse_packet_ready = 1;

        int8_t x = (int8_t)mouse_packet_buffer[1];
        int8_t y = (int8_t)mouse_packet_buffer[2];
        uint8_t status = mouse_packet_buffer[0];

        if (status & 0xC0) return;

        spinlock_acquire(&mouse_lock);

        mouse_global.x += apply_mouse_curve(x);
        mouse_global.y -= apply_mouse_curve(y);

        if (mouse_global.x < 0) mouse_global.x = 0;
        if (mouse_global.y < 0) mouse_global.y = 0;

        mouse_global.left_btn = (status & 0x01) ? 1 : 0;
        mouse_global.right_btn = (status & 0x02) ? 1 : 0;
        mouse_global.middle_btn = (status & 0x04) ? 1 : 0;
        mouse_state = mouse_global;

        spinlock_release(&mouse_lock);
    }
}

void mouse_init(void) {
    kprintf("🖱️  Initializing PS/2 Mouse...\n");

    spinlock_init(&mouse_lock);
    mouse_global.x = 400;
    mouse_global.y = 300;
    mouse_packet_index = 0;
    mouse_packet_ready = 0;

    mouse_write_command(PS2_CMD_ENABLE_AUX);

    mouse_write_command(PS2_CMD_READ_CONFIG);
    uint8_t config = mouse_read_data();
    config |= 0x02;
    config &= (uint8_t)~0x20;
    mouse_write_command(PS2_CMD_WRITE_CONFIG);
    mouse_write_data(config);

    mouse_write_device(PS2_DEV_SET_DEFAULTS);
    if (mouse_read_data() != PS2_ACK) {
        kprintf("⚠️  Mouse defaults command not acknowledged\n");
    }

    mouse_write_device(PS2_DEV_ENABLE_STREAMING);
    if (mouse_read_data() != PS2_ACK) {
        kprintf("⚠️  Mouse streaming enable not acknowledged\n");
    }

    kprintf("✅ PS/2 Mouse initialized at (%d, %d)\n", mouse_global.x, mouse_global.y);
}

void mouse_poll(void)
{
    spinlock_acquire(&mouse_lock);

    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL) {
        uint8_t status = inb(PS2_STATUS_PORT);
        if ((status & PS2_STATUS_AUX_DATA) == 0) break;

        spinlock_release(&mouse_lock);
        mouse_process_byte(inb(PS2_DATA_PORT));
        spinlock_acquire(&mouse_lock);
    }

    spinlock_release(&mouse_lock);
}

void mouse_handler(void) {
    mouse_process_byte(inb(PS2_DATA_PORT));
}

mouse_packet_t mouse_get_packet(void) {
    mouse_packet_t pkt = {0};
    spinlock_acquire(&mouse_lock);
    pkt.x_delta = mouse_packet_buffer[1];
    pkt.y_delta = -mouse_packet_buffer[2];
    pkt.left_btn = (mouse_packet_buffer[0] & 0x01) ? 1 : 0;
    pkt.right_btn = (mouse_packet_buffer[0] & 0x02) ? 1 : 0;
    pkt.middle_btn = (mouse_packet_buffer[0] & 0x04) ? 1 : 0;
    spinlock_release(&mouse_lock);
    return pkt;
}

int mouse_is_packet_ready(void) {
    return mouse_packet_ready;
}
