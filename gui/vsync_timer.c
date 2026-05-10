#include <timer.h>

void gfx_wait_vsync(uint32_t frames)
{
    if (frames == 0) return;
    uint64_t start = timer_get_ticks();
    uint64_t target = start + frames;
    while (timer_get_ticks() < target) {
        __asm__ volatile("pause");
    }
}
