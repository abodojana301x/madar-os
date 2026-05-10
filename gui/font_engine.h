#ifndef GUI_FONT_ENGINE_H
#define GUI_FONT_ENGINE_H

#include <types.h>

#define FONT_ALIGN_LEFT   0
#define FONT_ALIGN_CENTER 1
#define FONT_ALIGN_RIGHT  2

int font_init(void);
int font_draw_text(int x, int y, const char *text, uint32_t color, int rtl);
int font_draw_text_clipped(int x, int y, const char *text, uint32_t color,
                           int rtl, int clip_x, int clip_y, int clip_w, int clip_h);
int font_draw_text_aligned(int x, int y, int area_width, const char *text,
                           uint32_t color, int alignment);
int font_get_text_width(const char *text);
int font_get_width(void);
int font_get_height(void);
int font_get_advance(void);
int font_get_line_spacing(void);
int font_get_baseline(void);

#endif
