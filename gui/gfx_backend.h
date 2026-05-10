#ifndef GUI_GFX_BACKEND_H
#define GUI_GFX_BACKEND_H

#include <types.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t *front_buffer;
    uint32_t *back_buffer;
    int initialized;
} gui_surface_t;

int gfx_init(uint32_t width, uint32_t height);
void gfx_shutdown(void);
void gfx_clear(uint32_t color);
void gfx_draw_pixel(int x, int y, uint32_t color);
void gfx_draw_pixel_blend(int x, int y, uint32_t color);
void gfx_fill_rect(int x, int y, int w, int h, uint32_t color);
void gfx_fill_rect_blend(int x, int y, int w, int h, uint32_t color);
void gfx_copy_rect(int dx, int dy, int sx, int sy, int w, int h);
void gfx_swap_buffers(void);

int gfx_compositor_begin(void);
int gfx_compositor_add_layer(uint32_t *pixels, int width, int height, int has_alpha);
void gfx_compositor_composite(void);

uint32_t gfx_get_width(void);
uint32_t gfx_get_height(void);
gui_surface_t *gfx_surface(void);

#endif
