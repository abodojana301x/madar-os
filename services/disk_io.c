/*
 * Layer: 4 (System Services) - Disk I/O Implementation
 * File: disk_io.c
 * Purpose: Abstract ATA sector access for filesystems
 */

#include "disk_io.h"
#include <kprintf.h>
#include <string.h>

/* Include ATA driver from Layer 3 */
#include "drivers/ata.h"

int disk_io_init(void) {
    kprintf("💾 Disk I/O layer initialized\n");
    return 0;
}

int disk_io_read_sector(uint32_t lba, uint8_t *buffer) {
    if (!buffer) return -1;
    return ata_read_sector(lba, buffer);
}

int disk_io_write_sector(uint32_t lba, const uint8_t *buffer) {
    if (!buffer) return -1;
    return ata_write_sector(lba, buffer);
}

int disk_io_read_sectors(uint32_t lba, uint32_t count, uint8_t *buffer) {
    if (!buffer || count == 0) return -1;
    for (uint32_t i = 0; i < count; i++) {
        if (ata_read_sector(lba + i, buffer + i * SECTOR_SIZE) != 0) {
            return -1;
        }
    }
    return 0;
}

int disk_io_write_sectors(uint32_t lba, uint32_t count, const uint8_t *buffer) {
    if (!buffer || count == 0) return -1;
    for (uint32_t i = 0; i < count; i++) {
        if (ata_write_sector(lba + i, buffer + i * SECTOR_SIZE) != 0) {
            return -1;
        }
    }
    return 0;
}

