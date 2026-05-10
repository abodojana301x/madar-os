/* drivers/pci.c - PCI Enumeration for QEMU (Layer 3 Task 1) */

#include <types.h>
#include <kprintf.h>
#include <io.h>

#include "pci.h"
#include "device_list.h"

/* Global device list */
pci_device_t pci_devices[256];
uint32_t pci_device_count = 0;

uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t fn, uint16_t reg)
{
    uint32_t address = (1 << 31) | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3);
    outl(PCI_CONFIG_ADDRESS, address);
    /* Small delay */
    for (volatile int i = 0; i < 10; i++);
    return inl(PCI_CONFIG_DATA);
}

void pci_init(void) {
    kprintf("🧪 Scanning PCI Bus...\n");
    pci_enumerate();
}

void pci_enumerate(void)
{
    pci_device_count = 0;
    for (int bus = 0; bus < 256; bus++) {
        for (int dev = 0; dev < 32; dev++) {
            uint32_t vendor_dev = pci_read(bus, dev, 0, PCI_VENDOR_ID);
            uint16_t vendor = vendor_dev & 0xFFFF;
            uint16_t device = (vendor_dev >> 16) & 0xFFFF;
            if (vendor == 0xFFFF) continue;  /* No device */

            pci_devices[pci_device_count].bus = bus;
            pci_devices[pci_device_count].device = dev;
            pci_devices[pci_device_count].function = 0;
            pci_devices[pci_device_count].vendor_id = vendor;
            pci_devices[pci_device_count].device_id = device;
            pci_devices[pci_device_count].class_id = pci_read(bus, dev, 0, PCI_CLASS) >> 16;
            pci_devices[pci_device_count].bar0 = pci_read(bus, dev, 0, PCI_BAR0);
            
            /* Lookup name */
            pci_devices[pci_device_count].name = "Generic Device";
            
            pci_device_count++;
            if (pci_device_count >= 256) return;
        }
    }
}

void pci_print_devices(void) {
    kprintf("  Found %u PCI devices:\n", pci_device_count);
    for (uint32_t i = 0; i < pci_device_count; i++) {
        kprintf("    [%u:%u.0] %04X:%04X %s (BAR0=0x%08X)\n",
                pci_devices[i].bus, pci_devices[i].device,
                pci_devices[i].vendor_id, pci_devices[i].device_id,
                pci_devices[i].name, pci_devices[i].bar0);
    }
}
