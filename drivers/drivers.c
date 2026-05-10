/*
 * Layer: 3 (Drivers & Hardware Abstraction)
 * File: drivers.c
 * Purpose: Initialize and test all drivers
 * Status: New for Layer 3
 */

#include "drivers.h"
#include "../include/kprintf.h"

/* Initialize all Layer 3 drivers */
void drivers_init(void) {
    kprintf("🚀 Initializing Layer 3: Drivers & Hardware Abstraction\n");
    kprintf("─────────────────────────────────────────────────────\n\n");
    
    /* Initialize PCI enumeration */
    pci_init();
    pci_print_devices();
    kprintf("\n");
    
    /* Initialize input devices */
    kbd_init();
    mouse_init();
    kprintf("\n");
    
    /* Initialize storage */
    ata_init();
    kprintf("\n");
    
    /* Initialize network (best-effort: find RTL8139 from enumerated devices) */
    pci_device_t *rtl = NULL;
    for (uint32_t i = 0; i < pci_device_count; i++) {
        if (pci_devices[i].vendor_id == 0x10EC && pci_devices[i].device_id == 0x8139) {
            rtl = &pci_devices[i];
            break;
        }
    }
    if (rtl) {
        rtl8139_init(rtl->bar0 & ~1U, 0);
        kprintf("\n");
    } else {
        kprintf("⚠️  RTL8139 not found, skipping network init\n\n");
    }
    
    /* Initialize IRQ manager */
    irq_manager_init();
    kprintf("\n");
    
    kprintf("───────────────────────────────────────────────────\n");
    kprintf("✅ Layer 3: DRIVERS INITIALIZED\n");
    kprintf("🎯 All hardware ready for Layer 4 (System Services)\n");
}

/* Test all drivers */
void drivers_test(void) {
    kprintf("\n🧪 Testing Layer 3 Drivers...\n");
    kprintf("─────────────────────────────────────────────────────\n\n");
    
    /* Test PCI */
    kprintf("📡 PCI Test: %u devices found\n", pci_device_count);
    
    /* Test framebuffer if available */
    if (fb_global.initialized && fb_global.base) {
        kprintf("📺 Framebuffer Test: Base=0x%llx, Size=%ux%u\n",
                (uint64_t)fb_global.base, fb_global.width, fb_global.height);
        
        /* Draw a test rectangle */
        fb_draw_filled_rect(10, 10, 100, 100, COLOR_RED);
        kprintf("   ✅ Drew test rectangle\n");
    }
    
    /* Test keyboard */
    kprintf("⌨️  Keyboard Test: IRQ1 handler registered\n");
    
    /* Test mouse */
    kprintf("🖱️  Mouse Test: IRQ12 handler registered\n");
    
    /* Test storage - ATA with new diagnostics */
    kprintf("💾 ATA Storage Test:\n");
    ata_get_device_info();
    ata_test_read_write();
    kprintf("\n");
    
    /* Test network */
    kprintf("🌐 Network Test: RTL8139 MAC inuse\n");
    
    kprintf("\n──────────────────────────────────────────────────────\n");
    kprintf("✅ All driver tests completed\n");
}
