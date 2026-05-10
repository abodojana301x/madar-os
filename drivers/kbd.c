#include "kbd.h"
#include "../include/kprintf.h"
#include "../include/io.h"
#include "../include/spinlock.h"

static kbd_buffer_t kbd_buf = {0};
static spinlock_t kbd_lock;
static kbd_modifier_t kbd_mods = {0};

static const uint8_t scancode_us[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  /* L Ctrl */
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  /* L Shift */
    '\\','z','x','c','v','b','n','m',',','.','/',
    0,  /* R Shift */
    '*',
    0,  /* L Alt */
    ' ',
    0,  /* Caps Lock */
    0,0,0,0,0,0,0,0,0,0,  /* F1-F10 */
    0,  /* Num Lock */
    0,  /* Scroll Lock */
    '7','8','9','-',
    '4','5','6','+',
    '1','2','3','0','.'
};

static const uint8_t scancode_us_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,  /* L Ctrl */
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0,  /* L Shift */
    '|','Z','X','C','V','B','N','M','<','>','?',
    0,  /* R Shift */
    '*',
    0,  /* L Alt */
    ' ',
    0,  /* Caps Lock */
    0,0,0,0,0,0,0,0,0,0,  /* F1-F10 */
    0,  /* Num Lock */
    0,  /* Scroll Lock */
    '7','8','9','-',
    '4','5','6','+',
    '1','2','3','0','.'
};

static int kbd_try_extend(uint8_t scancode)
{
    return (scancode == 0xE0);
}

static int kbd_is_extended(uint8_t scancode)
{
    return (scancode == 0xE0);
}

static void kbd_update_modifiers(uint8_t scancode, int is_release)
{
    switch (scancode) {
    case 0x1D: kbd_mods.lctrl = !is_release; break;
    case 0x2A: kbd_mods.lshift = !is_release; break;
    case 0x36: kbd_mods.rshift = !is_release; break;
    case 0x38: kbd_mods.lalt = !is_release; break;
    case 0x1D: if (kbd_buf.extended) kbd_mods.rctrl = !is_release; break;
    case 0x38: if (kbd_buf.extended) kbd_mods.ralt = !is_release; break;
    case 0x3A: if (!is_release) kbd_mods.caps = !kbd_mods.caps; break;
    }
}

static uint8_t kbd_scancode_to_ascii(uint8_t scancode)
{
    if (scancode >= 128) return 0;

    uint8_t ascii;
    if (kbd_mods.lshift || kbd_mods.rshift) {
        ascii = scancode_us_shift[scancode];
    } else {
        ascii = scancode_us[scancode];
    }

    if (kbd_mods.caps && ascii >= 'a' && ascii <= 'z') {
        ascii = (uint8_t)(ascii - 'a' + 'A');
    } else if (kbd_mods.caps && ascii >= 'A' && ascii <= 'Z') {
        ascii = (uint8_t)(ascii - 'A' + 'a');
    }

    return ascii;
}

static void kbd_wait_write(void) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(PS2_STATUS_PORT) & 2)) return;
    }
}

void kbd_init(void) {
    kprintf("⌨️  Initializing PS/2 Keyboard...\n");

    spinlock_init(&kbd_lock);
    kbd_buf.head = 0;
    kbd_buf.tail = 0;
    kbd_buf.extended = 0;
    kbd_buf.trace = 0;

    kbd_wait_write();
    outb(PS2_CMD_PORT, PS2_CMD_DISABLE_KBD);

    while (inb(PS2_STATUS_PORT) & 1) {
        inb(PS2_DATA_PORT);
    }

    kbd_wait_write();
    outb(PS2_CMD_PORT, PS2_CMD_ENABLE_KBD);

    kprintf("✅ PS/2 Keyboard initialized\n");
}

static int kbd_put_scancode_ext(uint8_t scancode, uint8_t flags) {
    if (kbd_buf.head == kbd_buf.tail && kbd_buf.buffer_size == KBD_BUFFER_SIZE) {
        return 0;
    }
    uint32_t next = (kbd_buf.head + 1) % KBD_BUFFER_SIZE;
    if (next != kbd_buf.tail) {
        kbd_buf.buffer[kbd_buf.head] = scancode;
        kbd_buf.buffer_ext[kbd_buf.head] = flags;
        kbd_buf.head = next;
        kbd_buf.buffer_size++;
        return 1;
    }
    return 0;
}

void kbd_set_trace(int enabled)
{
    kbd_buf.trace = enabled ? 1 : 0;
}

void kbd_poll(void)
{
    spinlock_acquire(&kbd_lock);

    while (inb(PS2_STATUS_PORT) & 0x01) {
        uint8_t status = inb(PS2_STATUS_PORT);
        uint8_t data = inb(PS2_DATA_PORT);

        if (status & 0x20) break;

        if (kbd_is_extended(data)) {
            kbd_buf.extended = 1;
            continue;
        }

        int is_release = (data & 0x80) ? 1 : 0;
        uint8_t scancode = data & 0x7F;
        uint8_t flags = (uint8_t)((kbd_buf.extended ? 1 : 0) | (is_release ? 2 : 0));

        kbd_update_modifiers(scancode, is_release);
        kbd_put_scancode_ext(scancode, flags);
        kbd_buf.extended = 0;

        if (kbd_buf.trace) {
            kprintf("[KBD] scan=0x%02x '%c' %s ext=%d mods=%s%s%s%s\n",
                scancode,
                kbd_scancode_to_ascii(scancode) ? kbd_scancode_to_ascii(scancode) : ' ',
                is_release ? "UP" : "DN",
                kbd_buf.extended,
                kbd_mods.lctrl ? "LC" : "",
                kbd_mods.lshift ? "LS" : "",
                kbd_mods.lalt ? "LA" : "",
                kbd_mods.caps ? "CL" : "");
        }
    }

    spinlock_release(&kbd_lock);
}

uint8_t kbd_read_scancode(void) {
    if (kbd_buf.head == kbd_buf.tail) {
        return 0;
    }
    uint8_t c = kbd_buf.buffer[kbd_buf.tail];
    kbd_buf.tail = (kbd_buf.tail + 1) % KBD_BUFFER_SIZE;
    kbd_buf.buffer_size--;
    return c;
}

uint8_t kbd_read_ascii(void) {
    if (kbd_buf.head == kbd_buf.tail) {
        return 0;
    }
    uint8_t sc = kbd_buf.buffer[kbd_buf.tail];
    uint8_t ascii = kbd_scancode_to_ascii(sc);
    kbd_buf.tail = (kbd_buf.tail + 1) % KBD_BUFFER_SIZE;
    kbd_buf.buffer_size--;
    return ascii;
}

uint8_t kbd_has_data(void) {
    return kbd_buf.head != kbd_buf.tail;
}

int kbd_is_scancode_release(void) {
    if (kbd_buf.head == kbd_buf.tail) return 0;
    uint8_t sc = kbd_buf.buffer[kbd_buf.tail];
    return (sc & 0x80) ? 1 : 0;
}

kbd_modifier_t kbd_get_modifiers(void) {
    return kbd_mods;
}

void kbd_handler(void) {
    spinlock_acquire(&kbd_lock);

    uint8_t status = inb(PS2_STATUS_PORT);
    uint8_t data = inb(PS2_DATA_PORT);

    if (!(status & 0x20)) {
        if (kbd_is_extended(data)) {
            kbd_buf.extended = 1;
        } else {
            int is_release = (data & 0x80) ? 1 : 0;
            uint8_t scancode = data & 0x7F;
            uint8_t flags = (uint8_t)((kbd_buf.extended ? 1 : 0) | (is_release ? 2 : 0));
            kbd_update_modifiers(scancode, is_release);
            kbd_put_scancode_ext(scancode, flags);
            kbd_buf.extended = 0;
        }
    }

    spinlock_release(&kbd_lock);
}
