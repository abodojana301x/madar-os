#include "animation.h"
#include <stdlib.h>

/* Simple timer (would be replaced with proper system timer) */
static uint32_t g_frame_time = 0;

void animation_init(void)
{
    g_frame_time = 0;
}

animation_t *animation_create(animation_type_t type, uint32_t duration_ms)
{
    /* Static allocation - no malloc in kernel */
    static animation_t anim;
    
    anim.type = type;
    anim.duration = duration_ms;
    anim.easing = EASE_OUT_QUAD;
    anim.start_time = g_frame_time;
    anim.progress_int = 0;
    anim.active = 1;
    anim.value_from = 0;
    anim.value_to = 1000;
    anim.color_from = 0x00000000;
    anim.color_to = 0x00FFFFFF;
    
    return &anim;
}

void animation_update(animation_t *anim)
{
    if (!anim || !anim->active) return;
    
    uint32_t elapsed = g_frame_time - anim->start_time;
    
    if (elapsed >= anim->duration) {
        anim->progress_int = 1000;
        anim->active = 0;
    } else if (anim->duration > 0) {
        anim->progress_int = (elapsed * 1000) / anim->duration;
    }
}

/* Get animation progress as integer (0-1000) */
int animation_get_progress_int(animation_t *anim)
{
    if (!anim) return 0;
    int t = anim->progress_int;
    
    switch (anim->easing) {
        case EASE_LINEAR:
            return t;
        case EASE_OUT_QUAD: {
            int inv = 1000 - t;
            return 1000 - ((inv * inv) / 1000);
        }
        default:
            return t;
    }
}

int animation_is_finished(animation_t *anim)
{
    if (!anim) return 1;
    return anim->active == 0;
}
