/*
 * Layer: 7 (Final Integration) - Boot sequence
 * File: boot_sequence.c
 * Purpose: Order Layer 7 app startup after kernel, services, and GUI are ready.
 * Status: Phase 4 - Boot Flow Hardening with diagnostics
 */

#include "layer7.h"
#include "kprintf.h"
#include "../apps/shell.h"
#include "../apps/fileman.h"
#include "../apps/settings.h"
#include "../apps/taskmgr.h"
#include "../desktop/desktop.h"
#include "../include/acpi.h"
#include "../drivers/pci.h"
#include "../drivers/fb.h"

/* Boot diagnostic states */
typedef enum {
    BOOT_STATE_INIT = 0,
    BOOT_STATE_HARDWARE_OK,
    BOOT_STATE_ACPI_OK,
    BOOT_STATE_DRIVERS_OK,
    BOOT_STATE_SERVICES_OK,
    BOOT_STATE_COMPAT_OK,
    BOOT_STATE_GUI_OK,
    BOOT_STATE_APPS_OK,
    BOOT_STATE_FAILED
} boot_state_t;

static boot_state_t g_boot_state = BOOT_STATE_INIT;
static char g_boot_error[256] = {0};

/* Report hardware diagnostic */
void boot_report_hardware(void)
{
    kprintf("\n═══════════════════════════════════════════\n");
    kprintf("  BOOT DIAGNOSTICS - Hardware Report\n");
    kprintf("═══════════════════════════════════════════\n");

    /* Framebuffer status */
    if (fb_global.initialized) {
        kprintf("  ✅ Framebuffer: %ux%u %ubpp\n", fb_global.width, fb_global.height, fb_global.bpp);
    } else {
        kprintf("  ⚠️  Framebuffer: NOT initialized\n");
    }

    /* PCI devices */
    kprintf("  📡 PCI Devices: %u found\n", pci_device_count);
    for (uint32_t i = 0; i < pci_device_count && i < 10; i++) {
        kprintf("     [%02u:%02u.0] %04X:%04X\n",
                pci_devices[i].bus, pci_devices[i].device,
                pci_devices[i].vendor_id, pci_devices[i].device_id);
    }
    if (pci_device_count > 10) {
        kprintf("     ... and %u more\n", pci_device_count - 10);
    }

    /* ACPI status */
    acpi_info_t acpi_info;
    acpi_get_info(&acpi_info);
    if (acpi_info.valid) {
        kprintf("  ✅ ACPI: v%u.%u, %u CPUs, LAPIC 0x%X\n",
                acpi_info.revision >= 2 ? 2U : 1U, 0U,
                acpi_info.cpu_count, acpi_info.lapic_addr);
    } else {
        kprintf("  ⚠️  ACPI: Not available (using defaults)\n");
    }

    kprintf("═══════════════════════════════════════════\n\n");
}

int login_screen_show(void);
int init_gui_start_apps(void);
int stress_suite_run(void);

static void boot_set_error(const char *msg)
{
    g_boot_state = BOOT_STATE_FAILED;
    /* kprintf("❌ Boot Error: %s\n", msg); */
}

int layer7_init(void) {
    int result = 0;
    g_boot_state = BOOT_STATE_INIT;
    
    kprintf("\n═══════════════════════════════════════════\n");
    kprintf("  🚀 Layer 7 - Final Boot Sequence\n");
    kprintf("═══════════════════════════════════════════\n");

    /* Report hardware diagnostics */
    boot_report_hardware();
    g_boot_state = BOOT_STATE_HARDWARE_OK;

    kprintf("📦 Initializing applications...\n");
    shell_init();
    fileman_init();
    settings_init();
    taskmgr_init();
    g_boot_state = BOOT_STATE_APPS_OK;

    kprintf("🔐 Loading login screen...\n");
    if (login_screen_show() != 0) {
        boot_set_error("Login screen failed");
        kprintf("⚠️  Login screen unavailable - continuing\n");
    }

    kprintf("🖥️  Starting GUI applications...\n");
    if (init_gui_start_apps() != 0) {
        boot_set_error("GUI apps failed");
        kprintf("⚠️  GUI apps partial - continuing\n");
    }

    /* Self-test */
    if (layer7_selftest() != 0) {
        boot_set_error("Self-test failed");
        result = -1;
    }

    kprintf("═══════════════════════════════════════════\n");
    if (result == 0) {
        kprintf("  ✅ Boot sequence COMPLETE\n");
        g_boot_state = BOOT_STATE_APPS_OK;
    } else {
        kprintf("  ⚠️  Boot sequence completed with warnings\n");
    }
    kprintf("═══════════════════════════════════════════\n");

    return result;
}

int layer7_selftest(void) {
    int ok = 1;
    if (shell_selftest() != 0) { ok = 0; kprintf("⚠️  Shell self-test FAILED\n"); }
    if (fileman_selftest() != 0) { ok = 0; kprintf("⚠️  Fileman self-test FAILED\n"); }
    if (settings_selftest() != 0) { ok = 0; kprintf("⚠️  Settings self-test FAILED\n"); }
    if (desktop_shell_selftest() != 0) { ok = 0; kprintf("⚠️  Desktop self-test FAILED\n"); }
    extern int http_client_selftest(void);
    if (http_client_selftest() != 0) { ok = 0; kprintf("⚠️  HTTP client self-test FAILED\n"); }
    if (taskmgr_selftest() != 0) { ok = 0; kprintf("⚠️  Taskmgr self-test FAILED\n"); }
    if (stress_suite_run() != 0) { ok = 0; kprintf("⚠️  Stress suite FAILED\n"); }
    return ok ? 0 : -1;
}
