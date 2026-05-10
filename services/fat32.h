/*
 * Layer: 4 (System Services) - FAT32 Minimal Driver
 * File: fat32.h
 * Purpose: Read BPB, FAT tables, and cluster chains
 */

#ifndef FAT32_H
#define FAT32_H

#include <types.h>
#include "vfs.h"

#define FAT32_MAX_FILES     64
#define FAT32_SECTOR_SIZE   512
#define FAT32_NAME_LEN      11
#define FAT32_ENTRY_SIZE    32

/* FAT32 Boot Sector (BPB) */
typedef struct __attribute__((packed)) {
    uint8_t  jmp_boot[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  table_count;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t table_size_16;
    uint16_t sectors_per_track;
    uint16_t head_side_count;
    uint32_t hidden_sector_count;
    uint32_t total_sectors_32;
    uint32_t table_size_32;
    uint16_t extended_flags;
    uint16_t fat_version;
    uint32_t root_cluster;
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  fat_type_label[8];
} fat32_bpb_t;

/* Directory entry */
typedef struct __attribute__((packed)) {
    uint8_t  name[11];
    uint8_t  attr;
    uint8_t  ntres;
    uint8_t  crt_time_tenth;
    uint16_t crt_time;
    uint16_t crt_date;
    uint16_t lst_acc_date;
    uint16_t fst_clus_hi;
    uint16_t wrt_time;
    uint16_t wrt_date;
    uint16_t fst_clus_lo;
    uint32_t file_size;
} fat32_dentry_t;

/* FAT32 context */
typedef struct {
    fat32_bpb_t bpb;
    uint32_t fat_start;
    uint32_t data_start;
    uint32_t root_dir_sectors;
    uint32_t total_clusters;
    uint32_t sectors_per_fat;
    int mounted;
} fat32_ctx_t;

extern fat32_ctx_t fat32_ctx;

/* FAT32 API */
int fat32_init(void);
int fat32_mount(const char *device, vfs_inode_t *mount_point);
int fat32_read_cluster(uint32_t cluster, uint8_t *buffer);
int fat32_read_file(uint32_t cluster, uint32_t offset, uint8_t *buf, uint32_t len);
uint32_t fat32_next_cluster(uint32_t cluster);
int fat32_list_root(char *out_names, int max_entries);

#endif
