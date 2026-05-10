/*
 * Layer: 7 (User Applications)
 * File: terminal.c
 * Purpose: Simple terminal application for Madar OS
 *          - Text input/output
 *          - Basic command execution
 *          - UI through libmadar_ui
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"

/* Terminal state */
typedef struct {
    UIContainer* main_panel;
    Label* lbl_title;
    Label* lbl_output;
    Label* lbl_prompt;
    Button* btn_send;
    
    char input_buffer[256];
    char output_buffer[1024];
    int cursor_pos;
    int running;
} TerminalApp;

static TerminalApp terminal = {0};

/* ============ COMMAND EXECUTION ============ */

void terminal_append_output(const char* text) {
    if (!text || strlen(terminal.output_buffer) + strlen(text) >= sizeof(terminal.output_buffer)) {
        kprintf("[Terminal] Output buffer full\n");
        return;
    }
    strcat(terminal.output_buffer, text);
    strcat(terminal.output_buffer, "\n");
}

void terminal_clear_output(void) {
    memset(terminal.output_buffer, 0, sizeof(terminal.output_buffer));
}

void terminal_execute_command(void) {
    if (strlen(terminal.input_buffer) == 0) {
        return;
    }

    kprintf("[Terminal] Executing: %s\n", terminal.input_buffer);
    
    /* Display the command */
    terminal_append_output(terminal.input_buffer);

    /* Parse and execute */
    if (strcmp(terminal.input_buffer, "help") == 0) {
        terminal_append_output("=== Madar Terminal Help ===");
        terminal_append_output("Commands:");
        terminal_append_output("  help   - Show this help");
        terminal_append_output("  clear  - Clear screen");
        terminal_append_output("  about  - About Madar OS");
        terminal_append_output("  echo <text> - Echo text");
        terminal_append_output("  exit   - Close terminal");
    } 
    else if (strcmp(terminal.input_buffer, "clear") == 0) {
        terminal_clear_output();
    }
    else if (strcmp(terminal.input_buffer, "about") == 0) {
        terminal_append_output("=== Madar OS ===");
        terminal_append_output("A lightweight desktop OS");
        terminal_append_output("Stage 5 - Core Applications");
    }
    else if (strcmp(terminal.input_buffer, "exit") == 0) {
        terminal_append_output("Closing terminal...");
        terminal.running = 0;
    }
    else if (strncmp(terminal.input_buffer, "echo ", 5) == 0) {
        terminal_append_output(&terminal.input_buffer[5]);
    }
    else {
        terminal_append_output("Unknown command. Type 'help' for available commands.");
    }

    /* Clear input and reset cursor */
    memset(terminal.input_buffer, 0, sizeof(terminal.input_buffer));
    terminal.cursor_pos = 0;

    /* Update display */
    if (terminal.lbl_output) {
        terminal.lbl_output->text = terminal.output_buffer;
    }
}

/* ============ EVENT HANDLERS ============ */

void terminal_on_send_click(UIElement* el) {
    terminal_execute_command();
}

void terminal_on_input_hover(UIElement* el, int hovered) {
    /* Visual feedback for input area */
    (void)el;
    (void)hovered;
}

/* ============ UI SETUP ============ */

int terminal_init(void) {
    kprintf("[Terminal] Initializing terminal application...\n");

    ui_toolkit_init();

    /* Create main container */
    terminal.main_panel = ui_container_create(20, 20, 640, 480);
    if (!terminal.main_panel) {
        kprintf("[Terminal] ERROR: Failed to create main panel\n");
        return -1;
    }

    /* Title label */
    terminal.lbl_title = ui_label_create(40, 30, "Terminal - Madar OS");
    if (!terminal.lbl_title) return -1;
    ui_container_add_child(terminal.main_panel, (UIElement*)terminal.lbl_title);

    /* Output display area (shows command history) */
    terminal.lbl_output = ui_label_create(40, 70, "Welcome to Madar Terminal\nType 'help' for commands\n");
    if (!terminal.lbl_output) return -1;
    terminal.lbl_output->color = 0x00AAAAAA;
    ui_container_add_child(terminal.main_panel, (UIElement*)terminal.lbl_output);

    /* Prompt label */
    terminal.lbl_prompt = ui_label_create(40, 400, "madar# ");
    if (!terminal.lbl_prompt) return -1;
    ui_container_add_child(terminal.main_panel, (UIElement*)terminal.lbl_prompt);

    /* Send button */
    terminal.btn_send = ui_button_create(500, 395, 100, 35, "Execute");
    if (!terminal.btn_send) return -1;
    terminal.btn_send->base.on_click = terminal_on_send_click;
    ui_container_add_child(terminal.main_panel, (UIElement*)terminal.btn_send);

    /* Initialize buffers */
    memset(terminal.input_buffer, 0, sizeof(terminal.input_buffer));
    strcat(terminal.output_buffer, "Welcome to Madar Terminal\nType 'help' for commands\n");
    terminal.cursor_pos = 0;
    terminal.running = 1;

    kprintf("[Terminal] Initialized with %d UI elements\n", 
            terminal.main_panel->child_count);

    return 0;
}

/* ============ USER INPUT HANDLING ============ */

int terminal_handle_key(int key) {
    if (!terminal.running) return 0;

    /* Printable characters */
    if (key >= 32 && key < 127 && terminal.cursor_pos < 255) {
        terminal.input_buffer[terminal.cursor_pos++] = (char)key;
        terminal.input_buffer[terminal.cursor_pos] = '\0';
        kprintf("[Terminal] Input: %s\n", terminal.input_buffer);
        return 0;
    }

    /* Backspace */
    if (key == 8 && terminal.cursor_pos > 0) {
        terminal.input_buffer[--terminal.cursor_pos] = '\0';
        kprintf("[Terminal] Backspace\n");
        return 0;
    }

    /* Enter */
    if (key == 13) {
        terminal_execute_command();
        return 0;
    }

    return -1;
}

/* ============ RENDERING ============ */

int terminal_render(void) {
    if (!terminal.main_panel) return -1;
    ui_render_container(terminal.main_panel);
    return 0;
}

/* ============ INPUT HANDLING ============ */

int terminal_handle_click(int x, int y) {
    if (!terminal.main_panel) return -1;
    ui_handle_click((UIElement*)terminal.main_panel, x, y);
    return 0;
}

int terminal_handle_mouse_move(int x, int y) {
    if (!terminal.main_panel) return -1;
    ui_update_hover((UIElement*)terminal.main_panel, x, y);
    return 0;
}

/* ============ STATE MANAGEMENT ============ */

int terminal_is_running(void) {
    return terminal.running;
}

int terminal_shutdown(void) {
    kprintf("[Terminal] Shutting down...\n");

    if (terminal.main_panel) {
        free(terminal.main_panel);
    }

    kprintf("[Terminal] Shutdown complete\n");
    return 0;
}

/* ============ LAUNCHER INTERFACE ============ */

void terminal_launch(void) {
    kprintf("[Terminal] Launching Terminal Application...\n");

    if (terminal_init() != 0) {
        kprintf("[Terminal] FATAL: Could not initialize\n");
        return;
    }

    /* Simple main loop simulation */
    kprintf("[Terminal] Terminal ready for input\n");
}

int terminal_main(void) {
    terminal_launch();

    /* In a real environment, this would be in the event loop */
    while (terminal_is_running()) {
        terminal_render();
        /* Process events... */
        /* Small delay */
        for (volatile int i = 0; i < 100000; i++);
    }

    terminal_shutdown();

    return 0;
}
