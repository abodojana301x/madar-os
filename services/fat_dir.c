/*
 * Layer: 4 (System Services) - FAT32 Directory Operations
 * File: fat_dir.c
 * Purpose: Directory traversal and lookup for FAT32
 */

#include "fat32.h"
#include <kprintf.h>
#include <string.h>
#include <heap.h>

/* Read a directory entry by index from a cluster */
int fat32_readdir(uint32_t dir_cluster, uint32_t idx, fat32_dentry_t *out) {
    if (!out) return -1;

    uint32_t cluster_size = fat32_ctx.bpb.sectors_per_cluster * FAT32_SECTOR_SIZE;
    uint8_t *cluster_buf = (uint8_t *)kmalloc(cluster_size);
    if (!cluster_buf) return -1;

    uint32_t cluster = dir_cluster;
    uint32_t current_idx = 0;

    while (cluster >= 2) {
        if (fat32_read_cluster(cluster, cluster_buf) != 0) {
            kfree(cluster_buf);
            return -1;
        }

        for (uint32_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            fat32_dentry_t *ent = (fat32_dentry_t *)(cluster_buf + i);

            if (ent->name[0] == 0x00) {
                kfree(cluster_buf);
                return -1; /* No more entries */
            }
            if (ent->name[0] == 0xE5) continue;
            if (ent->attr == 0x0F) continue;
            if (ent->attr & 0x08) continue;

            if (current_idx == idx) {
                memcpy(out, ent, sizeof(fat32_dentry_t));
                kfree(cluster_buf);
                return 0;
            }
            current_idx++;
        }

        cluster = fat32_next_cluster(cluster);
    }

    kfree(cluster_buf);
    return -1;
}

/* Find a directory entry by name */
int fat32_lookup(uint32_t dir_cluster, const char *name, fat32_dentry_t *out) {
    if (!name || !out) return -1;

    uint32_t cluster_size = fat32_ctx.bpb.sectors_per_cluster * FAT32_SECTOR_SIZE;
    uint8_t *cluster_buf = (uint8_t *)kmalloc(cluster_size);
    if (!cluster_buf) return -1;

    uint32_t cluster = dir_cluster;

    while (cluster >= 2) {
        if (fat32_read_cluster(cluster, cluster_buf) != 0) {
            kfree(cluster_buf);
            return -1;
        }

        for (uint32_t i = 0; i < cluster_size; i += FAT32_ENTRY_SIZE) {
            fat32_dentry_t *ent = (fat32_dentry_t *)(cluster_buf + i);

            if (ent->name[0] == 0x00) {
                kfree(cluster_buf);
                return -1; /* Not found */
            }
            if (ent->name[0] == 0xE5) continue;
            if (ent->attr == 0x0F) continue;

            /* Compare name (simplified - no case conversion) */
            char entry_name[13];
            int j, k = 0;
            for (j = 0; j < 8 && ent->name[j] != ' '; j++) {
                entry_name[k++] = ent->name[j];
            }
            if (ent->name[8] != ' ') {
                entry_name[k++] = '.';
                for (int m = 8; m < 11 && ent->name[m] != ' '; m++) {
                    entry_name[k++] = ent->name[m];
                }
            }
            entry_name[k] = '\0';

            if (strcmp(entry_name, name) == 0) {
                memcpy(out, ent, sizeof(fat32_dentry_t));
                kfree(cluster_buf);
                return 0;
            }
        }

        cluster = fat32_next_cluster(cluster);
    }

    kfree(cluster_buf);
    return -1; /* Not found */
}
