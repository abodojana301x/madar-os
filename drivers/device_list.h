/* drivers/device_list.h - Common PCI Vendor/Device IDs for QEMU */

#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

/* Intel */
#define VENDOR_INTEL     0x8086
#define DEVICE_HOST_BRIDGE 0x1237  /* QEMU i440FX */

/* VGA */
#define VENDOR_BOCHS     0x1111
#define DEVICE_VGA       0x0001    /* Bochs VGA */

/* Network */
#define VENDOR_REALTEK   0x10ec
#define DEVICE_RTL8139   0x8139

/* Storage */
#define VENDOR_ATA       0x8086
#define DEVICE_ATA       0x2651    /* QEMU PIIX4 IDE */

/* PS/2 handled via i8042 controller, no PCI */

/* Struct for devices */
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    const char *name;
} pci_device_info_t;

#endif
