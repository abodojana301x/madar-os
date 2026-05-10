/*
 * Layer: 4 (System Services) - Security Model
 * File: security.h
 * Purpose: UID/GID and permission checks
 */

#ifndef SECURITY_H
#define SECURITY_H

#include <types.h>

#define MAX_UID     65536
#define MAX_GID     65536
#define ROOT_UID    0
#define ROOT_GID    0

/* Permission bits (Unix-style) */
#define PERM_READ   0x04
#define PERM_WRITE  0x02
#define PERM_EXEC   0x01

/* Permission macros */
#define PERM_OWNER_SHIFT 6
#define PERM_GROUP_SHIFT 3
#define PERM_OTHER_SHIFT 0

#define PERM_OWNER(mode) (((mode) >> PERM_OWNER_SHIFT) & 0x07)
#define PERM_GROUP(mode) (((mode) >> PERM_GROUP_SHIFT) & 0x07)
#define PERM_OTHER(mode) (((mode) >> PERM_OTHER_SHIFT) & 0x07)

#define MODE_SET(uid, gid, owner, group, other) \
    (((uid) << 16) | ((gid) << 8) | ((owner) << 6) | ((group) << 3) | (other))

typedef struct {
    uint32_t uid;
    uint32_t gid;
    uint16_t mode;  /* rwxrwxrwx + setuid bits */
} security_ctx_t;

/* Process security */
void security_init(void);
int security_check(const security_ctx_t *obj, uint32_t operation);
int security_set_uid(uint32_t uid);
int security_set_gid(uint32_t gid);
uint32_t security_get_uid(void);
uint32_t security_get_gid(void);

/* UID table management */
void uid_table_init(void);
int uid_register(uint32_t uid, const char *name);
const char *uid_lookup_name(uint32_t uid);

#endif

