/*
 * Layer: 7 (User Applications)
 * File: app_launcher.c
 * Purpose: Application registry and launcher for Madar OS
 *          - Manages available applications
 *          - Provides launcher interface
 *          - Compatible with desktop/start menu
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"

/* Forward declarations */
void terminal_launch(void);
void settings_launch(void);
void file_manager_launch(void);

/* Application entry */
typedef struct {
    char name[64];
    char description[256];
    void (*launch)(void);
} AppEntry;

/* Application registry */
static AppEntry app_registry[] = {
    {
        .name = "Terminal",
        .description = "Command-line interface",
        .launch = terminal_launch
    },
    {
        .name = "Settings",
        .description = "System settings",
        .launch = settings_launch
    },
    {
        .name = "File Manager",
        .description = "Browse files",
        .launch = file_manager_launch
    }
};

static const int app_count = sizeof(app_registry) / sizeof(app_registry[0]);

/* ============ APP REGISTRY API ============ */

const char* app_launcher_get_app_name(int index) {
    if (index < 0 || index >= app_count) return NULL;
    return app_registry[index].name;
}

const char* app_launcher_get_app_description(int index) {
    if (index < 0 || index >= app_count) return NULL;
    return app_registry[index].description;
}

int app_launcher_get_app_count(void) {
    return app_count;
}

int app_launcher_launch_by_name(const char* name) {
    if (!name) return -1;

    kprintf("[Launcher] Requesting app: %s\n", name);

    for (int i = 0; i < app_count; i++) {
        if (strcmp(app_registry[i].name, name) == 0) {
            kprintf("[Launcher] Launching: %s\n", app_registry[i].name);
            app_registry[i].launch();
            return 0;
        }
    }

    kprintf("[Launcher] App not found: %s\n", name);
    return -1;
}

int app_launcher_launch_by_index(int index) {
    if (index < 0 || index >= app_count) return -1;

    kprintf("[Launcher] Launching app %d: %s\n", index, app_registry[index].name);
    app_registry[index].launch();
    return 0;
}

/* ============ LAUNCHER UI ============ */

typedef struct {
    UIContainer* main_panel;
    Label* lbl_title;
    Button* btn_apps[3];
    Label* lbl_description;
    int running;
} LauncherUI;

static LauncherUI launcher_ui = {0};

void launcher_on_app_click(UIElement* el) {
    /* Find which button was clicked */
    for (int i = 0; i < app_count; i++) {
        if ((UIElement*)launcher_ui.btn_apps[i] == el) {
            kprintf("[Launcher] App button %d clicked\n", i);
            app_launcher_launch_by_index(i);
            return;
        }
    }
}

int launcher_ui_init(void) {
    kprintf("[Launcher UI] Initializing launcher interface...\n");

    ui_toolkit_init();

    /* Create main panel */
    launcher_ui.main_panel = ui_container_create(100, 100, 500, 400);
    if (!launcher_ui.main_panel) {
        kprintf("[Launcher UI] ERROR: Failed to create main panel\n");
        return -1;
    }

    /* Title */
    launcher_ui.lbl_title = ui_label_create(120, 110, "Madar OS - Applications");
    if (!launcher_ui.lbl_title) return -1;
    ui_container_add_child(launcher_ui.main_panel, (UIElement*)launcher_ui.lbl_title);

    /* Description */
    launcher_ui.lbl_description = ui_label_create(120, 350, "Select an application to launch");
    if (!launcher_ui.lbl_description) return -1;
    ui_container_add_child(launcher_ui.main_panel, (UIElement*)launcher_ui.lbl_description);

    /* Application buttons */
    for (int i = 0; i < app_count; i++) {
        launcher_ui.btn_apps[i] = ui_button_create(120, 170 + i * 70, 260, 50, 
                                                    app_registry[i].name);
        if (!launcher_ui.btn_apps[i]) return -1;

        launcher_ui.btn_apps[i]->base.on_click = launcher_on_app_click;
        ui_container_add_child(launcher_ui.main_panel, (UIElement*)launcher_ui.btn_apps[i]);
    }

    launcher_ui.running = 1;

    kprintf("[Launcher UI] Initialized with %d applications\n", app_count);

    return 0;
}

int launcher_ui_render(void) {
    if (!launcher_ui.main_panel) return -1;
    ui_render_container(launcher_ui.main_panel);
    return 0;
}

int launcher_ui_handle_click(int x, int y) {
    if (!launcher_ui.main_panel) return -1;
    ui_handle_click((UIElement*)launcher_ui.main_panel, x, y);
    return 0;
}

int launcher_ui_handle_mouse_move(int x, int y) {
    if (!launcher_ui.main_panel) return -1;
    ui_update_hover((UIElement*)launcher_ui.main_panel, x, y);
    return 0;
}

int launcher_ui_shutdown(void) {
    kprintf("[Launcher UI] Shutting down...\n");

    if (launcher_ui.main_panel) {
        free(launcher_ui.main_panel);
    }

    kprintf("[Launcher UI] Shutdown complete\n");

    return 0;
}

/* ============ MAIN LAUNCHER ============ */

int app_launcher_main(void) {
    kprintf("========================================\n");
    kprintf("  Madar OS - Application Launcher v0.1\n");
    kprintf("========================================\n\n");

    if (launcher_ui_init() != 0) {
        kprintf("[Launcher] FATAL: Could not initialize UI\n");
        return 1;
    }

    kprintf("[Launcher] Available applications:\n");
    for (int i = 0; i < app_count; i++) {
        kprintf("  %d. %s - %s\n", i + 1, 
                app_registry[i].name,
                app_registry[i].description);
    }

    kprintf("\n[Launcher] Launcher UI initialized and ready\n");

    /* Launcher UI would normally be embedded in the desktop environment */
    launcher_ui_shutdown();

    return 0;
}
