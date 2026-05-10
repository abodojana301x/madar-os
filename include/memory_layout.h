/* include/memory_layout.h - Layer 3 Driver Memory Layout */

#ifndef MEMORY_LAYOUT_H
#define MEMORY_LAYOUT_H

#define KERNEL_BASE       0xFFFFFFFF80000000
#define KERNEL_SIZE       0x200000    /* 2MB for kernel code */
#define KERNEL_STACK_SIZE 0x1000      /* 4KB per thread stack */

#define PHYSICAL_MEMORY  0x80000000   /* 2GB max */

/* Layer 2 Heap Region (physical, early boot) */
#define KERNEL_HEAP_START 0x00800000  /* 8MB */
#define KERNEL_HEAP_SIZE  0x01000000  /* 16MB */

/* Layer 3 Driver Reserves */
#define DRIVER_BASE       0x500000     /* 5MB physical for buffers */
#define NET_RX_RING       (DRIVER_BASE + 0x00000)  /* 64KB RX descriptors */
#define NET_TX_RING       (DRIVER_BASE + 0x10000)  /* 64KB TX */
#define FB_BACKBUFFER     (DRIVER_BASE + 0x20000)  /* Backup if needed */
#define ATA_SECTOR_CACHE  (DRIVER_BASE + 0x30000)  /* 1MB cache */
#define PS2_RING_SIZE     1024         /* Per device ring buffers */

/* Higher-half virtual for drivers */
#define DRIVER_VIRT_BASE  0xFFFF800050000000ULL

#endif
