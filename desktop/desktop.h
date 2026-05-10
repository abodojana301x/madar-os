#ifndef MADAR_DESKTOP_H
#define MADAR_DESKTOP_H

#include <types.h>

#define DESKTOP_MAX_APPS 8
#define DESKTOP_APP_NAME_MAX 32
#define DESKTOP_APP_TITLE_MAX 48

typedef struct {
    const char *id;
    const char *label;
    const char *title;
} desktop_app_t;

int desktop_shell_init(void);
void desktop_shell_draw_desktop(void);
void desktop_shell_draw(void);
int desktop_shell_handle_click(int x, int y);
int desktop_shell_selftest(void);

int desktop_panel_init(void);
void desktop_panel_draw(void);
int desktop_panel_handle_click(int x, int y);

int desktop_start_menu_init(void);
void desktop_start_menu_toggle(void);
void desktop_start_menu_close(void);
int desktop_start_menu_is_open(void);
void desktop_start_menu_draw(void);
int desktop_start_menu_handle_click(int x, int y);

int desktop_app_registry_init(void);
int desktop_app_count(void);
const desktop_app_t *desktop_app_get(int index);
const desktop_app_t *desktop_app_find(const char *id);
int desktop_app_launch(const char *id);

int desktop_session_init(void);
void desktop_session_note_launch(const char *id);
const char *desktop_session_recent(int index);
int desktop_session_open_count(void);

#endif
