/*
 * Layer: 7 (User Applications) - UI Toolkit Demo
 * File: ui_demo.c
 * Purpose: Demonstrates the libmadar_ui toolkit usage
 *          - Creating buttons, labels, containers
 *          - Handling events and layout
 *          - Integrating with GUI daemon
 */

#include "kprintf.h"
#include "string.h"
#include <stdlib.h>
#include "../gui/ui_toolkit.h"
#include "../gui/gfx_backend.h"

/* Demo application state */
static struct {
    UIContainer* root;
    Button* btn_hello;
    Button* btn_exit;
    Label* lbl_status;
    int click_count;
    int running;
} demo_app = {0};

/* ============ EVENT HANDLERS ============ */

void demo_on_hello_click(UIElement* el) {
    demo_app.click_count++;
    kprintf("[UI Demo] Hello button clicked! Count: %d\n", demo_app.click_count);
}

void demo_on_exit_click(UIElement* el) {
    kprintf("[UI Demo] Exit button clicked\n");
    demo_app.running = 0;
}

/* ============ UI SETUP ============ */

int ui_demo_init(void) {
    kprintf("[UI Demo] Initializing UI Toolkit demo...\n");

    /* Initialize toolkit */
    ui_toolkit_init();

    /* Create root container */
    demo_app.root = ui_container_create(10, 10, 400, 300);
    if (!demo_app.root) {
        kprintf("[UI Demo] ERROR: Failed to create root container\n");
        return -1;
    }

    /* Create status label */
    demo_app.lbl_status = ui_label_create(20, 20, "UI Toolkit Demo");
    if (!demo_app.lbl_status) {
        kprintf("[UI Demo] ERROR: Failed to create status label\n");
        return -1;
    }
    ui_container_add_child(demo_app.root, (UIElement*)demo_app.lbl_status);

    /* Create hello button */
    demo_app.btn_hello = ui_button_create(20, 50, 120, 40, "Say Hello");
    if (!demo_app.btn_hello) {
        kprintf("[UI Demo] ERROR: Failed to create hello button\n");
        return -1;
    }
    demo_app.btn_hello->base.on_click = demo_on_hello_click;
    ui_container_add_child(demo_app.root, (UIElement*)demo_app.btn_hello);

    /* Create exit button */
    demo_app.btn_exit = ui_button_create(160, 50, 120, 40, "Exit");
    if (!demo_app.btn_exit) {
        kprintf("[UI Demo] ERROR: Failed to create exit button\n");
        return -1;
    }
    demo_app.btn_exit->base.on_click = demo_on_exit_click;
    ui_container_add_child(demo_app.root, (UIElement*)demo_app.btn_exit);

    /* Arrange buttons horizontally */
    ui_layout_horizontal(demo_app.root, 20, 50, 10);

    demo_app.click_count = 0;
    demo_app.running = 1;

    kprintf("[UI Demo] Initialization complete\n");
    kprintf("[UI Demo] Root container: %dx%d at (%d,%d)\n",
            demo_app.root->base.width,
            demo_app.root->base.height,
            demo_app.root->base.x,
            demo_app.root->base.y);

    return 0;
}

/* ============ RENDERING ============ */

int ui_demo_render(void) {
    if (!demo_app.root) return -1;

    ui_render_container(demo_app.root);

    return 0;
}

/* ============ EVENT HANDLING ============ */

int ui_demo_handle_mouse_click(int x, int y) {
    if (!demo_app.root) return -1;

    kprintf("[UI Demo] Mouse click at (%d, %d)\n", x, y);
    ui_handle_click((UIElement*)demo_app.root, x, y);

    return 0;
}

int ui_demo_handle_mouse_move(int x, int y) {
    if (!demo_app.root) return -1;

    ui_update_hover((UIElement*)demo_app.root, x, y);

    return 0;
}

/* ============ APPLICATION LOOP ============ */

int ui_demo_update(void) {
    return demo_app.running ? 0 : 1;
}

int ui_demo_is_running(void) {
    return demo_app.running;
}

int ui_demo_shutdown(void) {
    kprintf("[UI Demo] Shutting down...\n");

    /* Free buttons */
    if (demo_app.btn_hello) {
        if (demo_app.btn_hello->text) {
            free(demo_app.btn_hello->text);
        }
        free(demo_app.btn_hello);
    }

    if (demo_app.btn_exit) {
        if (demo_app.btn_exit->text) {
            free(demo_app.btn_exit->text);
        }
        free(demo_app.btn_exit);
    }

    /* Free label */
    if (demo_app.lbl_status) {
        if (demo_app.lbl_status->text) {
            free(demo_app.lbl_status->text);
        }
        free(demo_app.lbl_status);
    }

    /* Free root container */
    if (demo_app.root) {
        free(demo_app.root);
    }

    kprintf("[UI Demo] Shutdown complete\n");

    return 0;
}

/* ============ MAIN ENTRY POINT ============ */

int main(int argc, char* argv[], char* envp[]) {
    (void)argc;
    (void)argv;
    (void)envp;

    kprintf("===============================\n");
    kprintf("  Madar OS - UI Toolkit Demo\n");
    kprintf("===============================\n\n");

    if (ui_demo_init() != 0) {
        kprintf("[UI Demo] FATAL: Initialization failed\n");
        return 1;
    }

    /* Main loop */
    while (ui_demo_is_running()) {
        /* Render */
        if (ui_demo_render() != 0) {
            kprintf("[UI Demo] ERROR: Render failed\n");
            break;
        }

        /* Update */
        if (ui_demo_update() != 0) {
            kprintf("[UI Demo] Demo completed\n");
            break;
        }

        /* Small delay to prevent busy-waiting */
        for (volatile int i = 0; i < 100000; i++);
    }

    ui_demo_shutdown();

    return 0;
}
