/*
 * Layer: 7 (User Applications)
 * File: settings_app.c
 * Purpose: Settings application for Madar OS
 *          - Adjust theme, language
 *          - Display system information
 *          - UI through libmadar_ui
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"

/* Global settings state */
typedef struct {
    int theme;         /* 0 = Light, 1 = Dark */
    int language;      /* 0 = EN, 1 = AR */
    int brightness;    /* 0-100 */
    int volume;        /* 0-100 */
} SystemSettings;

typedef struct {
    UIContainer* main_panel;
    
    Label* lbl_title;
    Label* lbl_theme;
    Label* lbl_language;
    Label* lbl_brightness;
    Label* lbl_volume;
    Label* lbl_sysinfo;
    
    Button* btn_toggle_theme;
    Button* btn_toggle_language;
    Button* btn_brightness_up;
    Button* btn_brightness_down;
    Button* btn_volume_up;
    Button* btn_volume_down;
    Button* btn_apply;
    Button* btn_cancel;

    int running;
} SettingsApp;

static SettingsApp settings_app = {0};

/* Global system settings */
static SystemSettings sys_settings = {
    .theme = 0,
    .language = 0,
    .brightness = 80,
    .volume = 70
};

/* ============ SETTINGS FUNCTIONS ============ */

void settings_apply(void) {
    kprintf("[Settings] Applying settings...\n");
    kprintf("  Theme: %s\n", sys_settings.theme ? "Dark" : "Light");
    kprintf("  Language: %s\n", sys_settings.language ? "عربي" : "English");
    kprintf("  Brightness: %d%%\n", sys_settings.brightness);
    kprintf("  Volume: %d%%\n", sys_settings.volume);
}

void settings_update_display(void) {
    if (!settings_app.lbl_theme) return;

    /* Update theme display */
    if (settings_app.lbl_theme->text) {
        settings_app.lbl_theme->text = sys_settings.theme ? "Theme: Dark" : "Theme: Light";
    }

    /* Update brightness display */
    static char brightness_str[32];
    snprintf(brightness_str, sizeof(brightness_str), "Brightness: %d%%", sys_settings.brightness);
    if (settings_app.lbl_brightness->text) {
        settings_app.lbl_brightness->text = brightness_str;
    }

    /* Update volume display */
    static char volume_str[32];
    snprintf(volume_str, sizeof(volume_str), "Volume: %d%%", sys_settings.volume);
    if (settings_app.lbl_volume->text) {
        settings_app.lbl_volume->text = volume_str;
    }
}

/* ============ EVENT HANDLERS ============ */

void settings_on_toggle_theme(UIElement* el) {
    sys_settings.theme = 1 - sys_settings.theme;
    kprintf("[Settings] Theme toggled to %s\n", 
            sys_settings.theme ? "Dark" : "Light");
    settings_update_display();
}

void settings_on_toggle_language(UIElement* el) {
    sys_settings.language = 1 - sys_settings.language;
    kprintf("[Settings] Language toggled to %s\n", 
            sys_settings.language ? "عربي" : "English");
}

void settings_on_brightness_up(UIElement* el) {
    if (sys_settings.brightness < 100) {
        sys_settings.brightness += 10;
        kprintf("[Settings] Brightness: %d%%\n", sys_settings.brightness);
        settings_update_display();
    }
}

void settings_on_brightness_down(UIElement* el) {
    if (sys_settings.brightness > 0) {
        sys_settings.brightness -= 10;
        kprintf("[Settings] Brightness: %d%%\n", sys_settings.brightness);
        settings_update_display();
    }
}

void settings_on_volume_up(UIElement* el) {
    if (sys_settings.volume < 100) {
        sys_settings.volume += 10;
        kprintf("[Settings] Volume: %d%%\n", sys_settings.volume);
        settings_update_display();
    }
}

void settings_on_volume_down(UIElement* el) {
    if (sys_settings.volume > 0) {
        sys_settings.volume -= 10;
        kprintf("[Settings] Volume: %d%%\n", sys_settings.volume);
        settings_update_display();
    }
}

void settings_on_apply(UIElement* el) {
    settings_apply();
}

void settings_on_cancel(UIElement* el) {
    kprintf("[Settings] Closing without save\n");
    settings_app.running = 0;
}

/* ============ UI SETUP ============ */

int settings_init(void) {
    kprintf("[Settings] Initializing settings application...\n");

    ui_toolkit_init();

    /* Create main panel */
    settings_app.main_panel = ui_container_create(50, 50, 500, 500);
    if (!settings_app.main_panel) {
        kprintf("[Settings] ERROR: Failed to create main panel\n");
        return -1;
    }

    /* Title */
    settings_app.lbl_title = ui_label_create(70, 70, "System Settings");
    if (!settings_app.lbl_title) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_title);

    /* Theme section */
    settings_app.lbl_theme = ui_label_create(70, 120, "Theme: Light");
    if (!settings_app.lbl_theme) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_theme);

    settings_app.btn_toggle_theme = ui_button_create(320, 110, 120, 35, "Toggle");
    if (!settings_app.btn_toggle_theme) return -1;
    settings_app.btn_toggle_theme->base.on_click = settings_on_toggle_theme;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_toggle_theme);

    /* Language section */
    settings_app.lbl_language = ui_label_create(70, 170, "Language: English");
    if (!settings_app.lbl_language) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_language);

    settings_app.btn_toggle_language = ui_button_create(320, 160, 120, 35, "عربي");
    if (!settings_app.btn_toggle_language) return -1;
    settings_app.btn_toggle_language->base.on_click = settings_on_toggle_language;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_toggle_language);

    /* Brightness section */
    settings_app.lbl_brightness = ui_label_create(70, 220, "Brightness: 80%");
    if (!settings_app.lbl_brightness) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_brightness);

    settings_app.btn_brightness_down = ui_button_create(320, 210, 55, 35, "-");
    if (!settings_app.btn_brightness_down) return -1;
    settings_app.btn_brightness_down->base.on_click = settings_on_brightness_down;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_brightness_down);

    settings_app.btn_brightness_up = ui_button_create(385, 210, 55, 35, "+");
    if (!settings_app.btn_brightness_up) return -1;
    settings_app.btn_brightness_up->base.on_click = settings_on_brightness_up;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_brightness_up);

    /* Volume section */
    settings_app.lbl_volume = ui_label_create(70, 270, "Volume: 70%");
    if (!settings_app.lbl_volume) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_volume);

    settings_app.btn_volume_down = ui_button_create(320, 260, 55, 35, "-");
    if (!settings_app.btn_volume_down) return -1;
    settings_app.btn_volume_down->base.on_click = settings_on_volume_down;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_volume_down);

    settings_app.btn_volume_up = ui_button_create(385, 260, 55, 35, "+");
    if (!settings_app.btn_volume_up) return -1;
    settings_app.btn_volume_up->base.on_click = settings_on_volume_up;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_volume_up);

    /* System info */
    settings_app.lbl_sysinfo = ui_label_create(70, 330, "System: Madar OS v1.0");
    if (!settings_app.lbl_sysinfo) return -1;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.lbl_sysinfo);

    /* Action buttons */
    settings_app.btn_apply = ui_button_create(200, 420, 100, 35, "Apply");
    if (!settings_app.btn_apply) return -1;
    settings_app.btn_apply->base.on_click = settings_on_apply;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_apply);

    settings_app.btn_cancel = ui_button_create(320, 420, 100, 35, "Cancel");
    if (!settings_app.btn_cancel) return -1;
    settings_app.btn_cancel->base.on_click = settings_on_cancel;
    ui_container_add_child(settings_app.main_panel, (UIElement*)settings_app.btn_cancel);

    settings_app.running = 1;

    kprintf("[Settings] Initialized with %d UI elements\n", 
            settings_app.main_panel->child_count);

    return 0;
}

/* ============ RENDERING ============ */

int settings_render(void) {
    if (!settings_app.main_panel) return -1;
    ui_render_container(settings_app.main_panel);
    return 0;
}

/* ============ INPUT HANDLING ============ */

int settings_handle_click(int x, int y) {
    if (!settings_app.main_panel) return -1;
    ui_handle_click((UIElement*)settings_app.main_panel, x, y);
    return 0;
}

int settings_handle_mouse_move(int x, int y) {
    if (!settings_app.main_panel) return -1;
    ui_update_hover((UIElement*)settings_app.main_panel, x, y);
    return 0;
}

/* ============ STATE MANAGEMENT ============ */

int settings_is_running(void) {
    return settings_app.running;
}

int settings_shutdown(void) {
    kprintf("[Settings] Shutting down...\n");

    if (settings_app.main_panel) {
        free(settings_app.main_panel);
    }

    kprintf("[Settings] Shutdown complete\n");

    return 0;
}

/* ============ LAUNCHER INTERFACE ============ */

void settings_launch(void) {
    kprintf("[Settings] Launching Settings Application...\n");

    if (settings_init() != 0) {
        kprintf("[Settings] FATAL: Could not initialize\n");
        return;
    }

    kprintf("[Settings] Settings ready\n");
}

int settings_main(void) {
    settings_launch();

    while (settings_is_running()) {
        settings_render();
        /* Process events... */
        for (volatile int i = 0; i < 100000; i++);
    }

    settings_shutdown();

    return 0;
}
