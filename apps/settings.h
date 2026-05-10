#ifndef SETTINGS_H
#define SETTINGS_H

int settings_init(void);
int settings_set(const char *key, const char *value);
int settings_get(const char *key, char *value, unsigned long size);
int settings_selftest(void);

#endif
