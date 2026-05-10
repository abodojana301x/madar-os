#ifndef GUI_ANIMATION_H
#define GUI_ANIMATION_H

#include <types.h>

/* Animation types */
typedef enum {
    ANIM_NONE = 0,
    ANIM_FADE_IN = 1,
    ANIM_FADE_OUT = 2,
    ANIM_SLIDE_LEFT = 3,
    ANIM_SLIDE_RIGHT = 4,
    ANIM_SLIDE_UP = 5,
    ANIM_SLIDE_DOWN = 6,
    ANIM_SCALE = 7,
    ANIM_COLOR_CHANGE = 8
} animation_type_t;

/* Easing functions */
typedef enum {
    EASE_LINEAR = 0,
    EASE_IN_QUAD = 1,
    EASE_OUT_QUAD = 2,
    EASE_IN_OUT_QUAD = 3
} easing_type_t;

/* Animation state */
typedef struct {
    animation_type_t type;
    easing_type_t easing;
    uint32_t start_time;
    uint32_t duration;  /* milliseconds */
    uint32_t progress_int;  /* 0-1000 */
    int active;
    
    /* Value ranges (as integers) */
    int value_from;
    int value_to;
    uint32_t color_from;
    uint32_t color_to;
} animation_t;

/* Initialize animation system */
void animation_init(void);

/* Create new animation */
animation_t *animation_create(animation_type_t type, uint32_t duration_ms);

/* Update animation state */
void animation_update(animation_t *anim);

/* Get current value as integer (0-1000 range) to avoid float */
int animation_get_progress_int(animation_t *anim);

/* Check if animation is finished */
int animation_is_finished(animation_t *anim);

/* Easing function */
float animation_apply_easing(float t, easing_type_t easing);

#endif
