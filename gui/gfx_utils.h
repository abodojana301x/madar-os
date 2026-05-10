#ifndef GUI_GFX_UTILS_H
#define GUI_GFX_UTILS_H

#include <types.h>
#include "gfx_backend.h"

#define ARGB_A(c) (((c) >> 24) & 0xFF)
#define ARGB_R(c) (((c) >> 16) & 0xFF)
#define ARGB_G(c) (((c) >> 8) & 0xFF)
#define ARGB_B(c) ((c) & 0xFF)
#define ARGB(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

void gfx_fill_rect_rounded(int x, int y, int w, int h, int radius, uint32_t color);
void gfx_fill_rect_rounded_blend(int x, int y, int w, int h, int radius, uint32_t color);
void gfx_stroke_rect_rounded(int x, int y, int w, int h, int radius, int thickness, uint32_t color);
void gfx_draw_shadow(int x, int y, int w, int h, int blur_radius, uint32_t shadow_color, int opacity);
void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color, int thickness);
void gfx_fill_circle(int cx, int cy, int radius, uint32_t color);
void gfx_stroke_circle(int cx, int cy, int radius, int thickness, uint32_t color);
void gfx_pixel_blend(int x, int y, uint32_t color, uint8_t alpha);
void gfx_draw_window(int x, int y, int w, int h, uint32_t bg_color,
                     uint32_t shadow_color, int corner_radius);

#endif
