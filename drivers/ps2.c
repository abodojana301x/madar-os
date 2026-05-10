#include "ps2.h"
#include "../include/spinlock.h"

/* Global keyboard ring buffer */
static ring_buffer_t kbd_buffer = {0};
static spinlock_t kbd_lock = SPINLOCK_INIT;

/* Global mouse state */
static ring_buffer_t mouse_buffer = {0};
static spinlock_t mouse_lock = SPINLOCK_INIT;
static uint8_t mouse_cycle = 0;
static mouse_packet_t mouse_current = {0};

/* I/O operations */
static inline uint8_t inb_port(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static inline void outb_port(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "d"(port));
}

/* Delay function */
static inline void io_delay(void) {
    asm volatile("jmp 1f; 1: jmp 2f; 2:");
    for (int i = 0; i < 1000; i++);
}

/* Wait for write operation to complete (input buffer empty) */
void ps2_wait_write(void) {
    for (int i = 0; i < 1000000; i++) {
        if ((inb_port(PS2_STATUS_PORT) & PS2_STATUS_INPUT_FULL) == 0) {
            return;
        }
        io_delay();
    }
}

/* Wait for read operation to complete (output buffer full) */
void ps2_wait_read(void) {
    for (int i = 0; i < 1000000; i++) {
        if (inb_port(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL) {
            return;
        }
        io_delay();
    }
}

/* Read a byte from PS/2 data port */
uint8_t ps2_read(void) {
    ps2_wait_read();
    return inb_port(PS2_DATA_PORT);
}

/* Write a byte to PS/2 data port */
void ps2_write(uint8_t data) {
    ps2_wait_write();
    outb_port(PS2_DATA_PORT, data);
}

/* Write a command to PS/2 controller */
void ps2_write_command(uint8_t cmd) {
    ps2_wait_write();
    outb_port(PS2_COMMAND_PORT, cmd);
}

/* Write a command to PS/2 device (keyboard or mouse) */
void ps2_device_write(uint8_t device, uint8_t cmd) {
    if (device == 2) {
        ps2_write_command(0xD4);  /* Write to port 2 (mouse) */
    }
    ps2_write(cmd);
}

/* Read response from PS/2 device */
uint8_t ps2_device_read(uint8_t device) {
    return ps2_read();
}

/* Initialize PS/2 Controller */
void ps2_init(void) {
    kprintf("🎮 Initializing PS/2 Controller...\n");
    
    /* Disable both ports first */
    ps2_write_command(PS2_CMD_DISABLE_PORT1);
    ps2_write_command(PS2_CMD_DISABLE_PORT2);
    
    /* Flush output buffer */
    while (inb_port(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL) {
        inb_port(PS2_DATA_PORT);
        io_delay();
    }
    
    /* Self-test controller */
    ps2_write_command(PS2_CMD_SELF_TEST);
    uint8_t self_test = ps2_read();
    if (self_test != 0x55) {
        kprintf("⚠️  PS/2 Controller self-test failed: 0x%x\n", self_test);
    }
    
    /* Enable keyboard port */
    ps2_write_command(PS2_CMD_ENABLE_PORT1);
    io_delay();
    
    /* Test keyboard port */
    ps2_write_command(PS2_CMD_PORT1_TEST);
    uint8_t kbd_test = ps2_read();
    if (kbd_test == 0x00) {
        kprintf("✅ PS/2 Keyboard port OK\n");
    } else {
        kprintf("⚠️  PS/2 Keyboard port test failed: 0x%x\n", kbd_test);
    }
    
    kprintf("✅ PS/2 Controller initialized\n");
}

/* Initialize keyboard */
void kbd_init(void) {
    kprintf("⌨️  Initializing Keyboard...\n");
    
    /* Reset keyboard */
    ps2_device_write(1, PS2_DEVICE_RESET);
    io_delay();
    uint8_t ack = ps2_read();
    
    if (ack == PS2_RESPONSE_ACK) {
        kprintf("✅ Keyboard initialized successfully\n");
    } else {
        kprintf("⚠️  Keyboard initialization warning: 0x%x\n", ack);
    }
    
    /* Enable keyboard */
    ps2_device_write(1, PS2_DEVICE_ENABLE);
    ps2_read();  /* Read ACK */
}

/* Initialize mouse */
void mouse_init(void) {
    kprintf("🖱️  Initializing Mouse...\n");
    
    /* Reset mouse */
    ps2_device_write(2, PS2_DEVICE_RESET);
    io_delay();
    uint8_t ack = ps2_read();
    
    if (ack == PS2_RESPONSE_ACK) {
        kprintf("✅ Mouse initialized successfully\n");
    } else {
        kprintf("⚠️  Mouse initialization warning: 0x%x\n", ack);
    }
    
    /* Enable mouse */
    ps2_device_write(2, PS2_DEVICE_ENABLE);
    ps2_read();  /* Read ACK */
}

/* Keyboard interrupt handler */
void ps2_handler_keyboard(void) {
    uint8_t scancode = inb_port(PS2_DATA_PORT);
    
    spinlock_acquire(&kbd_lock);
    
    if (kbd_buffer.count < PS2_BUFFER_SIZE) {
        kbd_buffer.buffer[kbd_buffer.tail] = scancode;
        kbd_buffer.tail = (kbd_buffer.tail + 1) % PS2_BUFFER_SIZE;
        kbd_buffer.count++;
    }
    
    spinlock_release(&kbd_lock);
}

/* Mouse interrupt handler */
void ps2_handler_mouse(void) {
    uint8_t data = inb_port(PS2_DATA_PORT);
    
    spinlock_acquire(&mouse_lock);
    
    if (mouse_cycle == 0) {
        mouse_current.buttons = data & 0x07;
        mouse_cycle = 1;
    } else if (mouse_cycle == 1) {
        mouse_current.x_delta = (int8_t)data;
        mouse_cycle = 2;
    } else if (mouse_cycle == 2) {
        mouse_current.y_delta = (int8_t)data;
        mouse_cycle = 0;
        
        /* Complete packet received */
        if (mouse_buffer.count < PS2_BUFFER_SIZE) {
            mouse_buffer.buffer[mouse_buffer.tail] = mouse_current.buttons;
            mouse_buffer.tail = (mouse_buffer.tail + 1) % PS2_BUFFER_SIZE;
            mouse_buffer.count++;
        }
    }
    
    spinlock_release(&mouse_lock);
}

/* Check if keyboard has input available */
uint32_t kbd_has_input(void) {
    spinlock_acquire(&kbd_lock);
    uint32_t has_data = kbd_buffer.count > 0;
    spinlock_release(&kbd_lock);
    return has_data;
}

/* Get a character from keyboard buffer */
uint8_t kbd_get_char(void) {
    spinlock_acquire(&kbd_lock);
    
    if (kbd_buffer.count == 0) {
        spinlock_release(&kbd_lock);
        return 0;
    }
    
    uint8_t char_code = kbd_buffer.buffer[kbd_buffer.head];
    kbd_buffer.head = (kbd_buffer.head + 1) % PS2_BUFFER_SIZE;
    kbd_buffer.count--;
    
    spinlock_release(&kbd_lock);
    return char_code;
}

/* Get mouse packet from buffer */
mouse_packet_t mouse_get_packet(void) {
    mouse_packet_t packet = {0};
    
    spinlock_acquire(&mouse_lock);
    
    if (mouse_buffer.count > 0) {
        packet.buttons = mouse_buffer.buffer[mouse_buffer.head];
        mouse_buffer.head = (mouse_buffer.head + 1) % PS2_BUFFER_SIZE;
        mouse_buffer.count--;
        packet.x_delta = mouse_current.x_delta;
        packet.y_delta = mouse_current.y_delta;
    }
    
    spinlock_release(&mouse_lock);
    return packet;
}
