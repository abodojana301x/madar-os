#include "rtl_support.h"
#include "font_engine.h"
#include <string.h>

static int is_arabic_utf8_continuation(unsigned char c)
{
    return (c >= 0x80 && c <= 0xBF);
}

static int is_arabic_utf8_start(unsigned char c)
{
    return (c >= 0xD8 && c <= 0xDF) || (c >= 0xC0 && c <= 0xD7);
}

text_direction_t rtl_detect_direction(const char *text)
{
    if (!text || *text == '\0') return TEXT_DIR_LTR;

    int ltr_count = 0;
    int rtl_count = 0;

    for (const char *p = text; *p != '\0'; p++) {
        unsigned char uc = (unsigned char)*p;
        if (is_arabic_utf8_start(uc)) {
            rtl_count++;
        } else if (uc >= 0x20 && uc < 0x7F) {
            ltr_count++;
        }
    }

    if (rtl_count > ltr_count) return TEXT_DIR_RTL;
    if (ltr_count > rtl_count) return TEXT_DIR_LTR;
    return TEXT_DIR_MIXED;
}

void rtl_draw_text(int x, int y, const char *text, uint32_t color, text_direction_t direction)
{
    if (!text || *text == '\0') return;

    if (direction == TEXT_DIR_RTL) {
        int text_width = rtl_get_text_width(text);
        font_draw_text(x - text_width, y, text, color, 1);
    } else {
        font_draw_text(x, y, text, color, 0);
    }
}

void rtl_draw_text_right_aligned(int x, int y, const char *text, uint32_t color)
{
    text_direction_t dir = rtl_detect_direction(text);

    if (dir == TEXT_DIR_RTL) {
        font_draw_text(x, y, text, color, 1);
    } else {
        int text_width = rtl_get_text_width(text);
        font_draw_text(x - text_width, y, text, color, 0);
    }
}

void rtl_draw_text_centered(int x, int y, int width, const char *text, uint32_t color)
{
    text_direction_t dir = rtl_detect_direction(text);
    int text_width = rtl_get_text_width(text);
    int offset = (width - text_width) / 2;

    if (dir == TEXT_DIR_RTL) {
        font_draw_text(x + offset + text_width, y, text, color, 1);
    } else {
        font_draw_text(x + offset, y, text, color, 0);
    }
}

int rtl_get_text_width(const char *text)
{
    if (!text) return 0;
    return font_get_text_width(text);
}

const char *rtl_shape_arabic(const char *text)
{
    return text;
}
