
/* drivers/pci.h - PCI Enumeration (Layer 3 Task 1) */

#ifndef PCI_H
#define PCI_H

#include <types.h>
#include "../include/types.h"
#include "../drivers/device_list.h"

/* PCI Config Space Registers */
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC

/* Common offsets */
#define PCI_VENDOR_ID       0x00
#define PCI_DEVICE_ID       0x02
#define PCI_COMMAND         0x04
#define PCI_STATUS          0x06
#define PCI_CLASS           0x0A
#define PCI_SUBCLASS        0x0B
#define PCI_BAR0            0x10
#define PCI_BAR1            0x14

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_id;
    uint8_t subclass;
    uint32_t bar0;
    const char *name;
} pci_device_t;

extern pci_device_t pci_devices[256];
extern uint32_t pci_device_count;

void pci_init(void);
void pci_enumerate(void);
uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t fn, uint16_t reg);
void pci_print_devices(void);

#endif
