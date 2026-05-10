#include "gfx_backend.h"
#include <heap.h>
#include <string.h>
#include <kprintf.h>
#include "../drivers/fb.h"

#define GFX_TARGET_WIDTH  1600
#define GFX_TARGET_HEIGHT 900
#define ARGB_A(c) (((c) >> 24) & 0xFF)
#define ARGB_R(c) (((c) >> 16) & 0xFF)
#define ARGB_G(c) (((c) >> 8) & 0xFF)
#define ARGB_B(c) ((c) & 0xFF)
#define ARGB(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

typedef struct {
    uint32_t *pixels;
    int width;
    int height;
    int has_alpha;
} compositor_layer_t;

#define MAX_COMPOSITOR_LAYERS 16

static gui_surface_t g_surface;
static compositor_layer_t g_composite_layers[MAX_COMPOSITOR_LAYERS];
static int g_layer_count = 0;

static uint32_t alpha_blend(uint32_t fg, uint32_t bg)
{
    uint8_t a = ARGB_A(fg);
    if (a == 255) return fg;
    if (a == 0) return bg;

    uint8_t r_fg = ARGB_R(fg), g_fg = ARGB_G(fg), b_fg = ARGB_B(fg);
    uint8_t r_bg = ARGB_R(bg), g_bg = ARGB_G(bg), b_bg = ARGB_B(bg);

    uint8_t r = (uint8_t)(((uint32_t)r_fg * a + (uint32_t)r_bg * (255 - a)) / 255);
    uint8_t g = (uint8_t)(((uint32_t)g_fg * a + (uint32_t)g_bg * (255 - a)) / 255);
    uint8_t b = (uint8_t)(((uint32_t)b_fg * a + (uint32_t)b_bg * (255 - a)) / 255);

    return ARGB(r, g, b, 255);
}

int gfx_init(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        width = GFX_TARGET_WIDTH;
        height = GFX_TARGET_HEIGHT;
    }
    uint64_t pixels = (uint64_t)width * (uint64_t)height;
    if (pixels > (16ULL * 1024ULL * 1024ULL)) return -1;

    memset(&g_surface, 0, sizeof(g_surface));
    g_surface.width = width;
    g_surface.height = height;

    g_surface.front_buffer = (uint32_t *)kmalloc((size_t)(pixels * sizeof(uint32_t)));
    g_surface.back_buffer  = (uint32_t *)kmalloc((size_t)(pixels * sizeof(uint32_t)));
    if (!g_surface.front_buffer || !g_surface.back_buffer) {
        if (g_surface.front_buffer) kfree(g_surface.front_buffer);
        if (g_surface.back_buffer) kfree(g_surface.back_buffer);
        memset(&g_surface, 0, sizeof(g_surface));
        return -1;
    }

    memset(g_surface.front_buffer, 0, (size_t)(pixels * sizeof(uint32_t)));
    memset(g_surface.back_buffer, 0, (size_t)(pixels * sizeof(uint32_t)));
    g_surface.initialized = 1;

    memset(g_composite_layers, 0, sizeof(g_composite_layers));
    g_layer_count = 0;

    kprintf("[gfx] Initialized %ux%u (%llu MB buffer)\n",
            width, height, (pixels * sizeof(uint32_t)) / (1024 * 1024));
    return 0;
}

void gfx_shutdown(void)
{
    if (g_surface.front_buffer) kfree(g_surface.front_buffer);
    if (g_surface.back_buffer) kfree(g_surface.back_buffer);
    memset(&g_surface, 0, sizeof(g_surface));
}

void gfx_clear(uint32_t color)
{
    if (!g_surface.initialized) return;
    uint64_t n = (uint64_t)g_surface.width * (uint64_t)g_surface.height;
    for (uint64_t i = 0; i < n; i++) {
        g_surface.back_buffer[i] = color;
    }
}

void gfx_draw_pixel(int x, int y, uint32_t color)
{
    if (!g_surface.initialized) return;
    if (x < 0 || y < 0) return;
    if ((uint32_t)x >= g_surface.width || (uint32_t)y >= g_surface.height) return;
    g_surface.back_buffer[(uint32_t)y * g_surface.width + (uint32_t)x] = color;
}

void gfx_draw_pixel_blend(int x, int y, uint32_t color)
{
    if (!g_surface.initialized) return;
    if (x < 0 || y < 0) return;
    if ((uint32_t)x >= g_surface.width || (uint32_t)y >= g_surface.height) return;
    uint32_t *p = &g_surface.back_buffer[(uint32_t)y * g_surface.width + (uint32_t)x];
    *p = alpha_blend(color, *p);
}

void gfx_fill_rect(int x, int y, int w, int h, uint32_t color)
{
    if (!g_surface.initialized || w <= 0 || h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w; if (x1 > (int)g_surface.width) x1 = (int)g_surface.width;
    int y1 = y + h; if (y1 > (int)g_surface.height) y1 = (int)g_surface.height;

    uint32_t stride = g_surface.width;
    uint32_t *base = g_surface.back_buffer;

    for (int yy = y0; yy < y1; yy++) {
        uint32_t *row = &base[(uint32_t)yy * stride + (uint32_t)x0];
        int row_pixels = x1 - x0;
        for (int xx = 0; xx < row_pixels; xx++) {
            row[xx] = color;
        }
    }
}

void gfx_fill_rect_blend(int x, int y, int w, int h, uint32_t color)
{
    if (!g_surface.initialized || w <= 0 || h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w; if (x1 > (int)g_surface.width) x1 = (int)g_surface.width;
    int y1 = y + h; if (y1 > (int)g_surface.height) y1 = (int)g_surface.height;

    uint32_t stride = g_surface.width;
    uint32_t *base = g_surface.back_buffer;

    for (int yy = y0; yy < y1; yy++) {
        uint32_t *row = &base[(uint32_t)yy * stride + (uint32_t)x0];
        int row_pixels = x1 - x0;
        for (int xx = 0; xx < row_pixels; xx++) {
            row[xx] = alpha_blend(color, row[xx]);
        }
    }
}

void gfx_copy_rect(int dx, int dy, int sx, int sy, int w, int h)
{
    if (!g_surface.initialized || w <= 0 || h <= 0) return;

    uint32_t stride = g_surface.width;
    uint32_t *buf = g_surface.back_buffer;

    for (int yy = 0; yy < h; yy++) {
        uint32_t *src = &buf[((uint32_t)sy + yy) * stride + (uint32_t)sx];
        uint32_t *dst = &buf[((uint32_t)dy + yy) * stride + (uint32_t)dx];
        memcpy(dst, src, (size_t)w * sizeof(uint32_t));
    }
}

void gfx_swap_buffers(void)
{
    if (!g_surface.initialized) return;
    uint64_t n = (uint64_t)g_surface.width * (uint64_t)g_surface.height;
    memcpy(g_surface.front_buffer, g_surface.back_buffer, (size_t)(n * sizeof(uint32_t)));

    if (!fb_global.initialized || !fb_global.base) return;
    if (fb_global.bpp != 32) return;

    uint32_t copy_w = g_surface.width < fb_global.width ? g_surface.width : fb_global.width;
    uint32_t copy_h = g_surface.height < fb_global.height ? g_surface.height : fb_global.height;
    uint32_t fb_stride = fb_global.pitch / 4U;

    for (uint32_t y = 0; y < copy_h; y++) {
        memcpy(&fb_global.base[y * fb_stride],
               &g_surface.front_buffer[y * g_surface.width],
               (size_t)copy_w * sizeof(uint32_t));
    }
}

int gfx_compositor_begin(void)
{
    g_layer_count = 0;
    return 0;
}

int gfx_compositor_add_layer(uint32_t *pixels, int width, int height, int has_alpha)
{
    if (g_layer_count >= MAX_COMPOSITOR_LAYERS) return -1;
    g_composite_layers[g_layer_count].pixels = pixels;
    g_composite_layers[g_layer_count].width = width;
    g_composite_layers[g_layer_count].height = height;
    g_composite_layers[g_layer_count].has_alpha = has_alpha;
    g_layer_count++;
    return g_layer_count - 1;
}

void gfx_compositor_composite(void)
{
    if (!g_surface.initialized || g_layer_count == 0) return;

    for (int i = 0; i < g_layer_count; i++) {
        compositor_layer_t *layer = &g_composite_layers[i];
        int cw = layer->width < (int)g_surface.width ? layer->width : (int)g_surface.width;
        int ch = layer->height < (int)g_surface.height ? layer->height : (int)g_surface.height;

        for (int y = 0; y < ch; y++) {
            for (int x = 0; x < cw; x++) {
                uint32_t fg = layer->pixels[(uint32_t)y * (uint32_t)layer->width + (uint32_t)x];
                if (layer->has_alpha) {
                    gfx_draw_pixel_blend(x, y, fg);
                } else if (ARGB_A(fg) > 0) {
                    gfx_draw_pixel(x, y, fg);
                }
            }
        }
    }
}

uint32_t gfx_get_width(void) { return g_surface.width; }
uint32_t gfx_get_height(void) { return g_surface.height; }
gui_surface_t *gfx_surface(void) { return &g_surface; }
