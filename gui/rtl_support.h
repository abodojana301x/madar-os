#ifndef GUI_RTL_SUPPORT_H
#define GUI_RTL_SUPPORT_H

#include <types.h>

/* Direction detection */
typedef enum {
    TEXT_DIR_LTR = 0,    /* Left-to-right (English) */
    TEXT_DIR_RTL = 1,    /* Right-to-left (Arabic) */
    TEXT_DIR_MIXED = 2   /* Mixed LTR/RTL */
} text_direction_t;

/* RTL text layout */
typedef struct {
    const char *text;
    text_direction_t direction;
    int start_x;
    int y;
    int width;
    int alignment;  /* 0=left, 1=center, 2=right */
} text_layout_t;

/* Detect if text is RTL (Arabic, Hebrew, etc.) */
text_direction_t rtl_detect_direction(const char *text);

/* Draw text with RTL support */
void rtl_draw_text(int x, int y, const char *text, uint32_t color, text_direction_t direction);

/* Draw text right-aligned for RTL */
void rtl_draw_text_right_aligned(int x, int y, const char *text, uint32_t color);

/* Draw text centered (handles both LTR and RTL) */
void rtl_draw_text_centered(int x, int y, int width, const char *text, uint32_t color);

/* Get text width (considering RTL) */
int rtl_get_text_width(const char *text);

/* Arabic character shaping (simplified version) */
const char *rtl_shape_arabic(const char *text);

#endif
