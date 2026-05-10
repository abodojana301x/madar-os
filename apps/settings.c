/*
 * Layer: 7 (User Applications) - Settings panel
 * File: settings.c
 * Purpose: Validated key=value control panel model with safe defaults.
 */

#include "settings.h"
#include "kprintf.h"
#include "string.h"

#define SETTINGS_MAX 8
#define SETTINGS_KEY 32
#define SETTINGS_VAL 64

typedef struct {
    char key[SETTINGS_KEY];
    char value[SETTINGS_VAL];
} setting_t;

static setting_t settings[SETTINGS_MAX];

static int valid_key(const char *key) {
    return key && (strcmp(key, "resolution") == 0 || strcmp(key, "dhcp") == 0 ||
                   strcmp(key, "language") == 0 || strcmp(key, "exe") == 0);
}

int settings_set(const char *key, const char *value) {
    if (!valid_key(key) || !value) return -1;
    for (int i = 0; i < SETTINGS_MAX; i++) {
        if (settings[i].key[0] == '\0' || strcmp(settings[i].key, key) == 0) {
            strncpy(settings[i].key, key, SETTINGS_KEY - 1);
            settings[i].key[SETTINGS_KEY - 1] = '\0';
            strncpy(settings[i].value, value, SETTINGS_VAL - 1);
            settings[i].value[SETTINGS_VAL - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

int settings_get(const char *key, char *value, unsigned long size) {
    if (!key || !value || size == 0) return -1;
    for (int i = 0; i < SETTINGS_MAX; i++) {
        if (strcmp(settings[i].key, key) == 0) {
            strncpy(value, settings[i].value, size - 1);
            value[size - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

int settings_init(void) {
    memset(settings, 0, sizeof(settings));
    settings_set("resolution", "1024x768");
    settings_set("dhcp", "on");
    settings_set("language", "auto");
    settings_set("exe", "safe");
    kprintf("Settings initialized\n");
    return 0;
}

int settings_selftest(void) {
    char value[SETTINGS_VAL];
    kprintf("Testing Settings Panel...\n");
    if (settings_set("resolution", "1280x720") != 0) return -1;
    if (settings_get("resolution", value, sizeof(value)) != 0) return -1;
    if (strcmp(value, "1280x720") != 0) return -1;
    if (settings_set("dhcp", "off") != 0) return -1;
    kprintf("Settings Panel PASSED\n");
    return 0;
}
