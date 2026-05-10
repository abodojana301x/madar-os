/*
 * Layer: 4 (System Services) - FAT32 Minimal Implementation
 * File: fat32.c
 * Purpose: Read BPB, FAT tables, and cluster chains
 */

#include "fat32.h"
#include "disk_io.h"
#include <kprintf.h>
#include <string.h>
#include <heap.h>

fat32_ctx_t fat32_ctx;

int fat32_init(void) {
    kprintf("📁 Initializing FAT32 driver...\n");
    memset(&fat32_ctx, 0, sizeof(fat32_ctx));
    kprintf("✅ FAT32 driver ready\n");
    return 0;
}

int fat32_mount(const char *device, vfs_inode_t *mount_point) {
    (void)device;
    (void)mount_point;
    uint8_t sector[FAT32_SECTOR_SIZE];

    /* Read boot sector (LBA 0) */
    if (disk_io_read_sector(0, sector) != 0) {
        kprintf("❌ FAT32: failed to read boot sector\n");
        return -1;
    }

    /* Parse BPB */
    memcpy(&fat32_ctx.bpb, sector, sizeof(fat32_bpb_t));

    /* Verify FAT32 signature */
    if (fat32_ctx.bpb.bytes_per_sector != 512) {
        kprintf("⚠️  FAT32: unexpected sector size %u\n", fat32_ctx.bpb.bytes_per_sector);
    }

    /* Calculate key offsets */
    fat32_ctx.fat_start = fat32_ctx.bpb.reserved_sector_count;
    fat32_ctx.sectors_per_fat = fat32_ctx.bpb.table_size_32;
    fat32_ctx.data_start = fat32_ctx.fat_start + (fat32_ctx.bpb.table_count * fat32_ctx.sectors_per_fat);
    fat32_ctx.total_clusters = (fat32_ctx.bpb.total_sectors_32 - fat32_ctx.data_start) / fat32_ctx.bpb.sectors_per_cluster;
    fat32_ctx.mounted = 1;

    kprintf("✅ FAT32 mounted:\n");
    kprintf("  Sectors/Cluster: %u\n", fat32_ctx.bpb.sectors_per_cluster);
    kprintf("  FAT start: %u\n", fat32_ctx.fat_start);
    kprintf("  Data start: %u\n", fat32_ctx.data_start);
    kprintf("  Total clusters: %u\n", fat32_ctx.total_clusters);
    kprintf("  Root cluster: %u\n", fat32_ctx.bpb.root_cluster);

    return 0;
}

uint32_t fat32_next_cluster(uint32_t cluster) {
    if (!fat32_ctx.mounted || cluster < 2) return 0;

    uint8_t sector[FAT32_SECTOR_SIZE];
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat32_ctx.fat_start + (fat_offset / FAT32_SECTOR_SIZE);
    uint32_t entry_offset = fat_offset % FAT32_SECTOR_SIZE;

    if (disk_io_read_sector(fat_sector, sector) != 0) {
        return 0xFFFFFFFF; /* Bad cluster */
    }

    uint32_t next = *((uint32_t *)(sector + entry_offset)) & 0x0FFFFFFF;
    if (next >= 0x0FFFFFF8) {
        return 0; /* End of chain */
    }
    return next;
}

int fat32_read_cluster(uint32_t cluster, uint8_t *buffer) {
    if (!fat32_ctx.mounted || cluster < 2 || !buffer) return -1;

    uint32_t sector = fat32_ctx.data_start + ((cluster - 2) * fat32_ctx.bpb.sectors_per_cluster);
    uint32_t count = fat32_ctx.bpb.sectors_per_cluster;

    return disk_io_read_sectors(sector, count, buffer);
}

int fat32_read_file(uint32_t start_cluster, uint32_t offset, uint8_t *buf, uint32_t len) {
    if (!fat32_ctx.mounted || !buf || len == 0) return -1;

    uint32_t cluster_size = fat32_ctx.bpb.sectors_per_cluster * FAT32_SECTOR_SIZE;
    uint32_t cluster = start_cluster;
    uint32_t skip = offset / cluster_size;
    uint32_t cluster_offset = offset % cluster_size;

    /* Skip to correct cluster */
    for (uint32_t i = 0; i < skip && cluster >= 2; i++) {
        cluster = fat32_next_cluster(cluster);
    }

    if (cluster < 2) return -1;

    uint32_t total_read = 0;
    uint8_t *cluster_buf = (uint8_t *)kmalloc(cluster_size);
    if (!cluster_buf) return -1;

    while (len > 0 && cluster >= 2) {
        if (fat32_read_cluster(cluster, cluster_buf) != 0) {
            kfree(cluster_buf);
            return -1;
        }

        uint32_t to_copy = cluster_size - cluster_offset;
        if (to_copy > len) to_copy = len;

        memcpy(buf + total_read, cluster_buf + cluster_offset, to_copy);
        total_read += to_copy;
        len -= to_copy;
        cluster_offset = 0;

        cluster = fat32_next_cluster(cluster);
    }

    kfree(cluster_buf);
    return (int)total_read;
}

static void fat32_trim_name(char *out, const uint8_t *name) {
    int i;
    for (i = 0; i < 8 && name[i] != ' '; i++) {
        out[i] = name[i];
    }
    if (name[8] != ' ') {
        out[i++] = '.';
        for (int j = 8; j < 11 && name[j] != ' '; j++) {
            out[i++] = name[j];
        }
    }
    out[i] = '\0';
}

int fat32_list_root(char *out_names, int max_entries) {
    if (!fat32_ctx.mounted || !out_names || max_entries <= 0) return -1;

    uint32_t cluster = fat32_ctx.bpb.root_cluster;
    uint32_t cluster_size = fat32_ctx.bpb.sectors_per_cluster * FAT32_SECTOR_SIZE;
    uint8_t *cluster_buf = (uint8_t *)kmalloc(cluster_size);
    if (!cluster_buf) return -1;

    int entry_count = 0;
    char *out_ptr = out_names;

    while (cluster >= 2 && entry_count < max_entries) {
        if (fat32_read_cluster(cluster, cluster_buf) != 0) break;

        for (uint32_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            fat32_dentry_t *ent = (fat32_dentry_t *)(cluster_buf + i);

            /* End of directory */
            if (ent->name[0] == 0x00) {
                kfree(cluster_buf);
                return entry_count;
            }
            /* Deleted entry */
            if (ent->name[0] == 0xE5) continue;
            /* Long filename entry */
            if (ent->attr == 0x0F) continue;
            /* Volume label */
            if (ent->attr & 0x08) continue;

            fat32_trim_name(out_ptr, ent->name);
            out_ptr += 13; /* Fixed width for simplicity */
            entry_count++;

            if (entry_count >= max_entries) break;
        }

        cluster = fat32_next_cluster(cluster);
    }

    kfree(cluster_buf);
    return entry_count;
}
