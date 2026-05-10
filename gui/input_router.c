#include "event_loop.h"
#include <string.h>

#define GUI_EVENT_Q 128

static gui_event_t g_q[GUI_EVENT_Q];
static uint32_t g_head = 0;
static uint32_t g_tail = 0;

void input_router_init(void)
{
    memset(g_q, 0, sizeof(g_q));
    g_head = g_tail = 0;
}

int input_router_push(const gui_event_t *ev)
{
    if (!ev) return -1;
    uint32_t next = (g_head + 1U) % GUI_EVENT_Q;
    if (next == g_tail) return -1;
    g_q[g_head] = *ev;
    g_head = next;
    return 0;
}

int input_router_pop(gui_event_t *ev)
{
    if (!ev) return -1;
    if (g_tail == g_head) return -1;
    *ev = g_q[g_tail];
    g_tail = (g_tail + 1U) % GUI_EVENT_Q;
    return 0;
}
