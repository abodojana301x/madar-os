#include "gfx_backend.h"

/* Minimal dirty rect policy: full present for stability */
void dbuf_present_full(void)
{
    gfx_swap_buffers();
}
