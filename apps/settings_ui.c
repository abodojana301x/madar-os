/*
 * Layer: 7 (User Applications) - UI-Based Settings
 * File: settings_ui.c
 * Purpose: Demonstrates UI Toolkit integration in a real application
 *          Settings application using standardized UI widgets
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"

/* Settings state */
static struct {
    UIContainer* main_panel;
    Button* btn_brightness;
    Button* btn_volume;
    Button* btn_language;
    Button* btn_apply;
    Label* lbl_title;
    Label* lbl_brightness;
    Label* lbl_volume;

    int brightness_level; /* 0-100 */
    int volume_level;     /* 0-100 */
    int current_lang;     /* 0=EN, 1=AR */
} settings_state = {
    .brightness_level = 80,
    .volume_level = 70,
    .current_lang = 0
};

/* ============ EVENT HANDLERS ============ */

void settings_on_brightness_click(UIElement* el) {
    settings_state.brightness_level = (settings_state.brightness_level + 10) % 110;
    kprintf("[Settings] Brightness: %d%%\n", settings_state.brightness_level);
}

void settings_on_volume_click(UIElement* el) {
    settings_state.volume_level = (settings_state.volume_level + 10) % 110;
    kprintf("[Settings] Volume: %d%%\n", settings_state.volume_level);
}

void settings_on_language_click(UIElement* el) {
    settings_state.current_lang = 1 - settings_state.current_lang;
    kprintf("[Settings] Language: %s\n",
            settings_state.current_lang ? "العربية" : "English");
}

void settings_on_apply_click(UIElement* el) {
    kprintf("[Settings] Applying settings...\n");
    kprintf("  Brightness: %d%%\n", settings_state.brightness_level);
    kprintf("  Volume: %d%%\n", settings_state.volume_level);
    kprintf("  Language: %s\n",
            settings_state.current_lang ? "العربية" : "English");
}

/* ============ UI SETUP ============ */

int settings_ui_init(void) {
    kprintf("[Settings UI] Initializing settings interface...\n");

    ui_toolkit_init();

    /* Create main panel */
    settings_state.main_panel = ui_container_create(50, 50, 500, 400);
    if (!settings_state.main_panel) {
        kprintf("[Settings UI] ERROR: Failed to create main panel\n");
        return -1;
    }

    /* Title label */
    settings_state.lbl_title = ui_label_create(70, 70, "System Settings");
    if (!settings_state.lbl_title) return -1;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.lbl_title);

    /* Brightness label and button */
    settings_state.lbl_brightness = ui_label_create(70, 120, "Brightness");
    if (!settings_state.lbl_brightness) return -1;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.lbl_brightness);

    settings_state.btn_brightness = ui_button_create(280, 110, 100, 40, "Increase");
    if (!settings_state.btn_brightness) return -1;
    settings_state.btn_brightness->base.on_click = settings_on_brightness_click;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.btn_brightness);

    /* Volume label and button */
    settings_state.lbl_volume = ui_label_create(70, 170, "Volume");
    if (!settings_state.lbl_volume) return -1;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.lbl_volume);

    settings_state.btn_volume = ui_button_create(280, 160, 100, 40, "Increase");
    if (!settings_state.btn_volume) return -1;
    settings_state.btn_volume->base.on_click = settings_on_volume_click;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.btn_volume);

    /* Language button */
    settings_state.btn_language = ui_button_create(70, 220, 150, 40, "EN / العربية");
    if (!settings_state.btn_language) return -1;
    settings_state.btn_language->base.on_click = settings_on_language_click;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.btn_language);

    /* Apply button */
    settings_state.btn_apply = ui_button_create(280, 330, 100, 40, "Apply");
    if (!settings_state.btn_apply) return -1;
    settings_state.btn_apply->base.on_click = settings_on_apply_click;
    ui_container_add_child(settings_state.main_panel, (UIElement*)settings_state.btn_apply);

    kprintf("[Settings UI] UI initialized with %d widgets\n",
            settings_state.main_panel->child_count);

    return 0;
}

int settings_ui_render(void) {
    if (!settings_state.main_panel) return -1;
    ui_render_container(settings_state.main_panel);
    return 0;
}

int settings_ui_handle_click(int x, int y) {
    if (!settings_state.main_panel) return -1;
    ui_handle_click((UIElement*)settings_state.main_panel, x, y);
    return 0;
}

int settings_ui_handle_hover(int x, int y) {
    if (!settings_state.main_panel) return -1;
    ui_update_hover((UIElement*)settings_state.main_panel, x, y);
    return 0;
}

int settings_ui_shutdown(void) {
    kprintf("[Settings UI] Shutting down...\n");

    if (settings_state.main_panel) {
        free(settings_state.main_panel);
    }

    kprintf("[Settings UI] Shutdown complete\n");

    return 0;
}
