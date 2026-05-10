#include "wm_core.h"

int gui_stress_test_run(void)
{
    int ids[50];
    for (int i = 0; i < 50; i++) {
        ids[i] = wm_create("Stress", 10 + (i % 10) * 20, 20 + (i % 8) * 14, 120, 70);
        if (ids[i] < 0) return -1;
    }

    for (int i = 0; i < 50; i++) {
        (void)wm_move((uint32_t)ids[i], 30 + i, 40 + i);
        (void)wm_resize((uint32_t)ids[i], 100 + (i % 5) * 10, 70 + (i % 4) * 6);
    }

    for (int i = 0; i < 50; i++) {
        (void)wm_close((uint32_t)ids[i]);
    }

    return 0;
}
