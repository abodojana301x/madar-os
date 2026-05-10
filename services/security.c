/*
 * Layer: 4 (System Services) - Security Model Implementation
 * File: security.c
 * Purpose: UID/GID checks and permission validation
 */

#include "security.h"
#include <process.h>
#include <kprintf.h>

static uint32_t current_uid = ROOT_UID;
static uint32_t current_gid = ROOT_GID;

void security_init(void) {
    kprintf("🔒 Initializing Security Model...\n");
    uid_table_init();
    current_uid = ROOT_UID;
    current_gid = ROOT_GID;
    kprintf("✅ Security Model initialized (root)\n");
}

int security_check(const security_ctx_t *obj, uint32_t operation) {
    if (!obj) return -1;

    /* Root always has access */
    if (current_uid == ROOT_UID) {
        return 0;
    }

    uint32_t perm_bits = 0;
    if (operation & PERM_READ)  perm_bits |= PERM_READ;
    if (operation & PERM_WRITE) perm_bits |= PERM_WRITE;
    if (operation & PERM_EXEC)  perm_bits |= PERM_EXEC;

    /* Check owner permissions */
    if (obj->uid == current_uid) {
        if ((PERM_OWNER(obj->mode) & perm_bits) == perm_bits) {
            return 0;
        }
        return -1;
    }

    /* Check group permissions */
    if (obj->gid == current_gid) {
        if ((PERM_GROUP(obj->mode) & perm_bits) == perm_bits) {
            return 0;
        }
        return -1;
    }

    /* Check other permissions */
    if ((PERM_OTHER(obj->mode) & perm_bits) == perm_bits) {
        return 0;
    }

    return -1; /* Permission denied */
}

int security_set_uid(uint32_t uid) {
    if (uid >= MAX_UID) return -1;
    /* Only root can change UID */
    if (current_uid != ROOT_UID && uid != current_uid) {
        return -1;
    }
    current_uid = uid;
    return 0;
}

int security_set_gid(uint32_t gid) {
    if (gid >= MAX_GID) return -1;
    /* Only root can change GID, or to a group you belong to */
    if (current_uid != ROOT_UID && gid != current_gid) {
        return -1;
    }
    current_gid = gid;
    return 0;
}

uint32_t security_get_uid(void) {
    return current_uid;
}

uint32_t security_get_gid(void) {
    return current_gid;
}

