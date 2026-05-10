#include <types.h>
#include <vga.h>
#include <serial.h>
#include <boot_diag.h>

#define BOOT_STAGE_MAX 32

static const char *boot_stage_names[BOOT_STAGE_MAX];
static int boot_stages_passed[BOOT_STAGE_MAX];
static int boot_stage_count = 0;
static int boot_has_failed = 0;
static char boot_error_msg[256];

void boot_diag_register_stage(const char *name)
{
    if (boot_stage_count < BOOT_STAGE_MAX) {
        boot_stage_names[boot_stage_count] = name;
        boot_stages_passed[boot_stage_count] = 0;
        boot_stage_count++;
    }
}

void boot_diag_report_stage(int stage_id, int success)
{
    if (stage_id >= 0 && stage_id < boot_stage_count) {
        boot_stages_passed[stage_id] = success;
        if (!success) {
            boot_has_failed = 1;
        }
    }
}

void boot_diag_fail(const char *msg)
{
    boot_has_failed = 1;
    int i = 0;
    while (msg[i] && i < 255) {
        boot_error_msg[i] = msg[i];
        i++;
    }
    boot_error_msg[i] = '\0';
}

int boot_diag_has_failed(void)
{
    return boot_has_failed;
}

void boot_diag_screen(void)
{
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    vga_clear();
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLUE);
    vga_puts("=== Madar OS Boot Diagnostics ===\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);

    for (int i = 0; i < boot_stage_count; i++) {
        vga_puts("  [");
        if (boot_stages_passed[i]) {
            vga_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLUE);
            vga_puts("PASS");
        } else {
            vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLUE);
            vga_puts("FAIL");
        }
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
        vga_puts("] ");
        vga_puts(boot_stage_names[i]);
        vga_putc('\n');
    }

    if (boot_error_msg[0]) {
        vga_puts("\nERROR: ");
        vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLUE);
        vga_puts(boot_error_msg);
        vga_putc('\n');
    }

    vga_puts("\nSystem halted. Press Ctrl+Alt+Del to reboot.\n");
}
