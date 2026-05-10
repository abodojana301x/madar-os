/*
 * Layer: 4 (System Services) - Disk I/O Abstraction
 * File: disk_io.h
 * Purpose: Abstract ATA sector access for filesystems
 */

#ifndef DISK_IO_H
#define DISK_IO_H

#include <types.h>

#define SECTOR_SIZE 512

int disk_io_init(void);
int disk_io_read_sector(uint32_t lba, uint8_t *buffer);
int disk_io_write_sector(uint32_t lba, const uint8_t *buffer);
int disk_io_read_sectors(uint32_t lba, uint32_t count, uint8_t *buffer);
int disk_io_write_sectors(uint32_t lba, uint32_t count, const uint8_t *buffer);

#endif

