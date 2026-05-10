/*
 * Layer: 4 (System Services) - UID Table
 * File: uid_table.c
 * Purpose: Maps UIDs to names
 */

#include "security.h"
#include <string.h>

#define MAX_USERS 32

typedef struct {
    uint32_t uid;
    char name[32];
    int used;
} uid_entry_t;

static uid_entry_t uid_table[MAX_USERS];

void uid_table_init(void) {
    memset(uid_table, 0, sizeof(uid_table));

    /* Register root */
    uid_register(ROOT_UID, "root");
}

int uid_register(uint32_t uid, const char *name) {
    if (!name || uid >= MAX_UID) return -1;

    /* Find existing or free slot */
    int free_slot = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (uid_table[i].used && uid_table[i].uid == uid) {
            /* Update existing */
            uid_table[i].uid = uid;
            strncpy(uid_table[i].name, name, 31);
            uid_table[i].name[31] = '\0';
            return 0;
        }
        if (!uid_table[i].used && free_slot == -1) {
            free_slot = i;
        }
    }

    if (free_slot == -1) return -1; /* Table full */

    uid_table[free_slot].uid = uid;
    strncpy(uid_table[free_slot].name, name, 31);
    uid_table[free_slot].name[31] = '\0';
    uid_table[free_slot].used = 1;
    return 0;
}

const char *uid_lookup_name(uint32_t uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (uid_table[i].used && uid_table[i].uid == uid) {
            return uid_table[i].name;
        }
    }
    return "unknown";
}

