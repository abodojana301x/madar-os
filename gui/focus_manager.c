#include <types.h>

static uint32_t g_focus = 0;

void focus_set(uint32_t id) { g_focus = id; }
uint32_t focus_get(void) { return g_focus; }
