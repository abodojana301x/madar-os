#include <types.h>

static uint32_t g_cache_hits = 0;
static uint32_t g_cache_miss = 0;

void glyph_cache_touch(int hit)
{
    if (hit) g_cache_hits++;
    else g_cache_miss++;
}

uint32_t glyph_cache_hits(void) { return g_cache_hits; }
uint32_t glyph_cache_miss(void) { return g_cache_miss; }
