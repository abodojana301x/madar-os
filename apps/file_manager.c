/*
 * Layer: 7 (User Applications)
 * File: file_manager.c
 * Purpose: Read-only file manager for Madar OS
 *          - Display files and directories
 *          - Navigate directories (basic)
 *          - UI through libmadar_ui
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"

/* File entry structure */
typedef struct {
    char name[256];
    int is_dir;
    int size;
} FileEntry;

typedef struct {
    UIContainer* main_panel;
    
    Label* lbl_title;
    Label* lbl_path;
    Label* lbl_file_list;
    
    Button* btn_back;
    Button* btn_refresh;
    Button* btn_home;
    Button* btn_exit;

    FileEntry files[64];
    int file_count;
    
    char current_path[512];
    int running;
} FileManagerApp;

static FileManagerApp file_manager = {0};

/* ============ FILE OPERATIONS ============ */

void file_manager_add_file(const char* name, int is_dir, int size) {
    if (file_manager.file_count >= 64) return;
    
    strncpy(file_manager.files[file_manager.file_count].name, name, 255);
    file_manager.files[file_manager.file_count].is_dir = is_dir;
    file_manager.files[file_manager.file_count].size = size;
    file_manager.file_count++;
}

void file_manager_load_default_files(void) {
    file_manager.file_count = 0;

    /* Root directory contents */
    file_manager_add_file("..", 1, 0);
    file_manager_add_file("boot", 1, 0);
    file_manager_add_file("apps", 1, 0);
    file_manager_add_file("drivers", 1, 0);
    file_manager_add_file("gui", 1, 0);
    file_manager_add_file("kernel", 1, 0);
    file_manager_add_file("system", 1, 0);
    file_manager_add_file("README.md", 0, 2048);
    file_manager_add_file("Makefile", 0, 1024);
    file_manager_add_file("linker.ld", 0, 512);
}

void file_manager_load_apps_files(void) {
    file_manager.file_count = 0;

    file_manager_add_file("..", 1, 0);
    file_manager_add_file("terminal.c", 0, 5120);
    file_manager_add_file("settings_app.c", 0, 4096);
    file_manager_add_file("file_manager.c", 0, 3584);
    file_manager_add_file("launcher.c", 0, 2048);
    file_manager_add_file("shell.c", 0, 4096);
    file_manager_add_file("builtins.c", 0, 2560);
}

void file_manager_update_file_list(void) {
    static char file_list_str[512];
    int offset = 0;

    memset(file_list_str, 0, sizeof(file_list_str));

    for (int i = 0; i < file_manager.file_count && i < 10; i++) {
        const char* icon = file_manager.files[i].is_dir ? "[D] " : "[F] ";
        offset += snprintf(&file_list_str[offset], 
                         sizeof(file_list_str) - offset,
                         "%s%s\n", 
                         icon, 
                         file_manager.files[i].name);
    }

    if (file_manager.lbl_file_list) {
        file_manager.lbl_file_list->text = file_list_str;
    }
}

/* ============ EVENT HANDLERS ============ */

void file_manager_on_back(UIElement* el) {
    kprintf("[FileManager] Back button clicked\n");
    if (strcmp(file_manager.current_path, "/") != 0) {
        strcpy(file_manager.current_path, "/");
        file_manager_load_default_files();
        file_manager_update_file_list();
    }
}

void file_manager_on_refresh(UIElement* el) {
    kprintf("[FileManager] Refresh button clicked\n");
    if (strcmp(file_manager.current_path, "/") == 0) {
        file_manager_load_default_files();
    } else if (strcmp(file_manager.current_path, "/apps") == 0) {
        file_manager_load_apps_files();
    }
    file_manager_update_file_list();
}

void file_manager_on_home(UIElement* el) {
    kprintf("[FileManager] Home button clicked\n");
    strcpy(file_manager.current_path, "/");
    file_manager_load_default_files();
    file_manager_update_file_list();
}

void file_manager_on_exit(UIElement* el) {
    kprintf("[FileManager] Exit button clicked\n");
    file_manager.running = 0;
}

/* ============ UI SETUP ============ */

int file_manager_init(void) {
    kprintf("[FileManager] Initializing file manager application...\n");

    ui_toolkit_init();

    /* Create main panel */
    file_manager.main_panel = ui_container_create(30, 30, 700, 520);
    if (!file_manager.main_panel) {
        kprintf("[FileManager] ERROR: Failed to create main panel\n");
        return -1;
    }

    /* Title */
    file_manager.lbl_title = ui_label_create(50, 40, "File Manager - Madar OS");
    if (!file_manager.lbl_title) return -1;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.lbl_title);

    /* Path display */
    file_manager.lbl_path = ui_label_create(50, 80, "Location: /");
    if (!file_manager.lbl_path) return -1;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.lbl_path);

    /* File list area */
    file_manager.lbl_file_list = ui_label_create(50, 130, 
        "[D] boot\n[D] apps\n[D] drivers\n[D] gui\n[D] kernel\n"
        "[D] system\n[F] README.md\n[F] Makefile");
    if (!file_manager.lbl_file_list) return -1;
    file_manager.lbl_file_list->color = 0x00CCCCCC;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.lbl_file_list);

    /* Navigation buttons */
    file_manager.btn_back = ui_button_create(50, 430, 80, 35, "Back");
    if (!file_manager.btn_back) return -1;
    file_manager.btn_back->base.on_click = file_manager_on_back;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.btn_back);

    file_manager.btn_home = ui_button_create(140, 430, 80, 35, "Home");
    if (!file_manager.btn_home) return -1;
    file_manager.btn_home->base.on_click = file_manager_on_home;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.btn_home);

    file_manager.btn_refresh = ui_button_create(230, 430, 100, 35, "Refresh");
    if (!file_manager.btn_refresh) return -1;
    file_manager.btn_refresh->base.on_click = file_manager_on_refresh;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.btn_refresh);

    file_manager.btn_exit = ui_button_create(600, 430, 80, 35, "Exit");
    if (!file_manager.btn_exit) return -1;
    file_manager.btn_exit->base.on_click = file_manager_on_exit;
    ui_container_add_child(file_manager.main_panel, (UIElement*)file_manager.btn_exit);

    /* Initialize */
    strcpy(file_manager.current_path, "/");
    file_manager_load_default_files();
    file_manager.running = 1;

    kprintf("[FileManager] Initialized with %d UI elements\n",
            file_manager.main_panel->child_count);

    return 0;
}

/* ============ RENDERING ============ */

int file_manager_render(void) {
    if (!file_manager.main_panel) return -1;
    ui_render_container(file_manager.main_panel);
    return 0;
}

/* ============ INPUT HANDLING ============ */

int file_manager_handle_click(int x, int y) {
    if (!file_manager.main_panel) return -1;
    ui_handle_click((UIElement*)file_manager.main_panel, x, y);
    return 0;
}

int file_manager_handle_mouse_move(int x, int y) {
    if (!file_manager.main_panel) return -1;
    ui_update_hover((UIElement*)file_manager.main_panel, x, y);
    return 0;
}

/* ============ STATE MANAGEMENT ============ */

int file_manager_is_running(void) {
    return file_manager.running;
}

int file_manager_shutdown(void) {
    kprintf("[FileManager] Shutting down...\n");

    if (file_manager.main_panel) {
        free(file_manager.main_panel);
    }

    kprintf("[FileManager] Shutdown complete\n");

    return 0;
}

/* ============ LAUNCHER INTERFACE ============ */

void file_manager_launch(void) {
    kprintf("[FileManager] Launching File Manager Application...\n");

    if (file_manager_init() != 0) {
        kprintf("[FileManager] FATAL: Could not initialize\n");
        return;
    }

    kprintf("[FileManager] File Manager ready\n");
}

int file_manager_main(void) {
    file_manager_launch();

    while (file_manager_is_running()) {
        file_manager_render();
        /* Process events... */
        for (volatile int i = 0; i < 100000; i++);
    }

    file_manager_shutdown();

    return 0;
}
