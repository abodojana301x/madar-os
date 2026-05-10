#include <panic.h>
#include <kprintf.h>
#include <serial.h>
#include <vga.h>

static void panic_draw_banner(void)
{
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_clear();
    vga_puts("KERNEL PANIC\n");
    vga_puts("System halted.\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void panic(const char *file, int line, const char *message)
{
    panic_draw_banner();
    kprintf("PANIC: %s\n", message ? message : "(null)");
    kprintf("AT: %s:%d\n", file ? file : "(unknown)", line);
    serial_puts("Kernel halted after panic.\n");

    __asm__ volatile("cli");
    while (1) {
        __asm__ volatile("hlt");
    }
}
