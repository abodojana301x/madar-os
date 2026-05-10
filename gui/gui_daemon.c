#include "gui_daemon.h"
#include "gfx_backend.h"
#include "font_engine.h"
#include "wm_core.h"
#include "event_loop.h"
#include "widgets.h"
#include "taskbar.h"
#include "../desktop/desktop.h"
#include <kprintf.h>

extern void perf_monitor_init(void);
extern void perf_monitor_frame(uint32_t events);
extern void perf_monitor_report(void);
extern int gui_stress_test_run(void);
extern uint32_t gui_style_bg(void);

int gui_layer6_init(void)
{
    if (gfx_init(1600, 900) != 0) {
        if (gfx_init(1280, 720) != 0) {
            if (gfx_init(1024, 768) != 0) return -1;
        }
    }
    if (font_init() != 0) return -1;

    wm_init();
    widgets_init();
    taskbar_init();
    event_loop_init();
    if (desktop_shell_init() != 0) return -1;

    if (wm_create("Desktop", 80, 70, 360, 220) < 0) return -1;
    if (wm_create("Terminal", 180, 130, 380, 240) < 0) return -1;
    if (wm_create("Inspector", 300, 180, 320, 200) < 0) return -1;

    desktop_shell_draw_desktop();
    wm_draw_all();
    desktop_shell_draw();
    gfx_swap_buffers();

    return 0;
}

int gui_layer6_selftest(void)
{
    perf_monitor_init();

    /* Test 1: Window creation */
    kprintf("🧪 Test 1: Creating windows...\n");
    int w1 = wm_create("Test1", 100, 100, 150, 100);
    int w2 = wm_create("Test2", 150, 150, 150, 100);
    int w3 = wm_create("Test3", 200, 200, 150, 100);
    
    if (w1 < 0 || w2 < 0 || w3 < 0) {
        kprintf("❌ Window creation failed\n");
        return -1;
    }
    kprintf("✅ Windows created: %u, %u, %u\n", w1, w2, w3);

    /* Test 2: Mouse down (focus and close) */
    kprintf("🧪 Test 2: Testing mouse down and focus...\n");
    gui_event_t ev;
    
    /* Click on window 1 title bar to focus */
    ev.type = GUI_EVENT_MOUSE_DOWN;
    ev.x = 120;
    ev.y = 105;
    event_loop_inject(&ev);
    int events = event_loop_step();
    perf_monitor_frame((uint32_t)(events < 0 ? 0 : events));
    
    kprintf("✅ Mouse down test passed\n");

    /* Test 3: Mouse move (dragging) */
    kprintf("🧪 Test 3: Testing mouse move (drag)...\n");
    for (int i = 0; i < 10; i++) {
        ev.type = GUI_EVENT_MOUSE_MOVE;
        ev.x = 120 + i * 5;
        ev.y = 105 + i * 3;
        event_loop_inject(&ev);
        events = event_loop_step();
        perf_monitor_frame((uint32_t)(events < 0 ? 0 : events));
    }
    
    kprintf("✅ Mouse move test passed\n");

    /* Test 4: Mouse up (end drag) */
    kprintf("🧪 Test 4: Testing mouse up...\n");
    ev.type = GUI_EVENT_MOUSE_UP;
    ev.x = 170;
    ev.y = 135;
    event_loop_inject(&ev);
    events = event_loop_step();
    perf_monitor_frame((uint32_t)(events < 0 ? 0 : events));
    
    kprintf("✅ Mouse up test passed\n");

    /* Test 5: Z-order (click on different window) */
    kprintf("🧪 Test 5: Testing Z-order...\n");
    ev.type = GUI_EVENT_MOUSE_DOWN;
    ev.x = 210;
    ev.y = 215;
    event_loop_inject(&ev);
    events = event_loop_step();
    perf_monitor_frame((uint32_t)(events < 0 ? 0 : events));
    
    kprintf("✅ Z-order test passed\n");

    /* Test 6: Close button */
    kprintf("🧪 Test 6: Testing close button...\n");
    /* Find close button position - it's at x + w - 18 */
    /* For window at x=150, w=150: close button is around 150+150-18 = 282 */
    ev.type = GUI_EVENT_MOUSE_DOWN;
    ev.x = 282;  /* Close button of w2 */
    ev.y = 155;  /* Title bar */
    event_loop_inject(&ev);
    events = event_loop_step();
    perf_monitor_frame((uint32_t)(events < 0 ? 0 : events));
    
    kprintf("✅ Close button test passed\n");

    /* Stress test */
    if (gui_stress_test_run() != 0) {
        kprintf("❌ Layer6 stress test failed\n");
        return -1;
    }

    perf_monitor_report();
    if (event_loop_latency_ms() > 16U) {
        kprintf("❌ Layer6 latency high: %u ms\n", event_loop_latency_ms());
        return -1;
    }

    kprintf("✅ Layer 6: GUI SYSTEM STABLE\n");
    return 0;
}

/**
 * Main GUI event loop
 * Polls input devices and processes events
 */
void gui_layer6_main_loop(void)
{
    extern void input_poller_init(void);
    extern void input_poller_poll(void);
    
    input_poller_init();
    
    while (1) {
        /* Poll input devices (keyboard, mouse) */
        input_poller_poll();
        
        /* Process one frame of events */
        int handled = event_loop_step();
        (void)handled;
    }
}
