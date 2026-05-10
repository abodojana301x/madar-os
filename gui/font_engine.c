#include "font_engine.h"
#include "gfx_backend.h"
#include <string.h>

#define FONT_WIDTH  8
#define FONT_HEIGHT 13
#define FONT_ADVANCE 9

typedef struct {
    int width;
    int height;
    int advance;
    int line_spacing;
    int baseline;
} font_metrics_t;

static font_metrics_t g_font_metrics;
static int g_font_initialized = 0;

#include "font_data_8x13.h"

int font_init(void)
{
    g_font_metrics.width = FONT_WIDTH;
    g_font_metrics.height = FONT_HEIGHT;
    g_font_metrics.advance = FONT_ADVANCE;
    g_font_metrics.line_spacing = FONT_HEIGHT + 4;
    g_font_metrics.baseline = FONT_HEIGHT - 2;
    g_font_initialized = 1;
    return 0;
}

void font_get_metrics(font_metrics_t *metrics)
{
    if (metrics) *metrics = g_font_metrics;
}

int font_get_width(void) { return FONT_WIDTH; }
int font_get_height(void) { return FONT_HEIGHT; }
int font_get_advance(void) { return FONT_ADVANCE; }
int font_get_line_spacing(void) { return g_font_metrics.line_spacing; }
int font_get_baseline(void) { return g_font_metrics.baseline; }

static const uint8_t *font_get_glyph(char ch)
{
    unsigned char uc = (unsigned char)ch;
    if (uc < 32 || uc > 126) uc = 127;
    return font_data_8x13[(size_t)(uc - 32)];
}

static void font_draw_glyph(int x, int y, char ch, uint32_t color)
{
    const uint8_t *glyph = font_get_glyph(ch);
    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (bits & (0x80 >> col)) {
                gfx_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

int font_draw_text(int x, int y, const char *text, uint32_t color, int rtl)
{
    if (!text) return 0;

    int len = (int)strlen(text);
    int pen_x = x;

    if (rtl) {
        for (int i = len - 1; i >= 0; i--) {
            char ch = text[i];
            if (ch == '\n') continue;
            font_draw_glyph(pen_x, y, ch, color);
            pen_x -= FONT_ADVANCE;
        }
    } else {
        for (int i = 0; i < len; i++) {
            char ch = text[i];
            if (ch == '\n') {
                y += g_font_metrics.line_spacing;
                pen_x = x;
                continue;
            }
            font_draw_glyph(pen_x, y, ch, color);
            pen_x += FONT_ADVANCE;
        }
    }
    return len;
}

int font_draw_text_clipped(int x, int y, const char *text, uint32_t color,
                           int rtl, int clip_x, int clip_y, int clip_w, int clip_h)
{
    if (!text) return 0;
    int len = (int)strlen(text);

    for (int i = 0; i < len; i++) {
        char ch = text[i];
        int gx = x + i * FONT_ADVANCE;

        if (rtl) gx = x - i * FONT_ADVANCE;

        if (gx + FONT_WIDTH < clip_x) continue;
        if (gx > clip_x + clip_w) break;
        if (y + FONT_HEIGHT < clip_y) continue;
        if (y > clip_y + clip_h) continue;

        font_draw_glyph(gx, y, ch, color);
    }
    return len;
}

int font_draw_text_aligned(int x, int y, int area_width, const char *text,
                           uint32_t color, int alignment)
{
    if (!text) return 0;
    int text_width = font_get_text_width(text);

    int start_x = x;
    switch (alignment) {
    case FONT_ALIGN_CENTER:
        start_x = x + (area_width - text_width) / 2;
        break;
    case FONT_ALIGN_RIGHT:
        start_x = x + area_width - text_width;
        break;
    case FONT_ALIGN_LEFT:
    default:
        start_x = x;
        break;
    }

    return font_draw_text(start_x, y, text, color, 0);
}

int font_get_text_width(const char *text)
{
    if (!text) return 0;
    return (int)strlen(text) * FONT_ADVANCE;
}
