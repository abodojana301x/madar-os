#include <types.h>
#include <timer.h>
#include <kprintf.h>

static uint64_t g_start_tick = 0;
static uint32_t g_frames = 0;
static uint32_t g_events = 0;

void perf_monitor_init(void)
{
    g_start_tick = timer_get_ticks();
    g_frames = 0;
    g_events = 0;
}

void perf_monitor_frame(uint32_t events)
{
    g_frames++;
    g_events += events;
}

void perf_monitor_report(void)
{
    uint64_t dt = timer_get_ticks() - g_start_tick;
    if (dt == 0) dt = 1;
    uint64_t fps = ((uint64_t)g_frames * 100ULL) / dt;
    kprintf("[gui-perf] frames=%u events=%u fps~%llu latency<16ms target\n", g_frames, g_events, fps);
}
