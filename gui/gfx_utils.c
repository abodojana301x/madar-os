#include "gfx_utils.h"
#include "gfx_backend.h"

void gfx_fill_rect_rounded(int x, int y, int w, int h, int radius, uint32_t color)
{
    if (radius <= 0) {
        gfx_fill_rect(x, y, w, h, color);
        return;
    }

    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    gfx_fill_rect(x + radius, y, w - 2 * radius, h, color);

    gfx_fill_rect(x, y + radius, radius, h - 2 * radius, color);
    gfx_fill_rect(x + w - radius, y + radius, radius, h - 2 * radius, color);

    int rad_sq = radius * radius;

    for (int py = 0; py < radius; py++) {
        int line_len = 0;
        for (int px = 0; px < radius; px++) {
            if (px * px + py * py <= rad_sq) {
                line_len = px + 1;
            }
        }
        if (line_len > 0) {
            gfx_fill_rect(x, y + py, line_len, 1, color);
            gfx_fill_rect(x + w - line_len, y + h - py - 1, line_len, 1, color);
        }
    }

    for (int py = 0; py < radius; py++) {
        for (int px = 0; px < radius; px++) {
            if (px * px + py * py <= rad_sq) {
                gfx_fill_rect(x + w - px - 1, y + py, 1, 1, color);
                gfx_fill_rect(x + px, y + h - py - 1, 1, 1, color);
                break;
            }
        }
    }
}

void gfx_fill_rect_rounded_blend(int x, int y, int w, int h, int radius, uint32_t color)
{
    if (ARGB_A(color) == 255) {
        gfx_fill_rect_rounded(x, y, w, h, radius, color);
        return;
    }
    if (radius <= 0) {
        gfx_fill_rect_blend(x, y, w, h, color);
        return;
    }

    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    gfx_fill_rect_blend(x + radius, y, w - 2 * radius, h, color);
    gfx_fill_rect_blend(x, y + radius, radius, h - 2 * radius, color);
    gfx_fill_rect_blend(x + w - radius, y + radius, radius, h - 2 * radius, color);

    int rad_sq = radius * radius;
    for (int py = 0; py < radius; py++) {
        int line_len = 0;
        for (int px = 0; px < radius; px++) {
            if (px * px + py * py <= rad_sq) {
                line_len = px + 1;
            }
        }
        if (line_len > 0) {
            gfx_fill_rect_blend(x, y + py, line_len, 1, color);
            gfx_fill_rect_blend(x + w - line_len, y + h - py - 1, line_len, 1, color);
        }
    }
    for (int py = 0; py < radius; py++) {
        for (int px = 0; px < radius; px++) {
            if (px * px + py * py <= rad_sq) {
                gfx_draw_pixel_blend(x + w - px - 1, y + py, color);
                gfx_draw_pixel_blend(x + px, y + h - py - 1, color);
                break;
            }
        }
    }
}

void gfx_stroke_rect_rounded(int x, int y, int w, int h, int radius, int thickness, uint32_t color)
{
    if (thickness <= 0) return;

    if (radius <= 0) {
        for (int t = 0; t < thickness; t++) {
            gfx_fill_rect(x + t, y + t, w - 2*t, 1, color);
            gfx_fill_rect(x + t, y + h - 1 - t, w - 2*t, 1, color);
            gfx_fill_rect(x + t, y + t, 1, h - 2*t, color);
            gfx_fill_rect(x + w - 1 - t, y + t, 1, h - 2*t, color);
        }
        return;
    }

    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    gfx_fill_rect(x + radius, y, w - 2 * radius, thickness, color);
    gfx_fill_rect(x + radius, y + h - thickness, w - 2 * radius, thickness, color);
    gfx_fill_rect(x, y + radius, thickness, h - 2 * radius, color);
    gfx_fill_rect(x + w - thickness, y + radius, thickness, h - 2 * radius, color);

    for (int t = 0; t < thickness; t++) {
        int cur_rad = radius - t;
        if (cur_rad <= 0) continue;
        int cr_sq = cur_rad * cur_rad;

        for (int dy = 0; dy < cur_rad; dy++) {
            for (int dx = 0; dx < cur_rad; dx++) {
                if (dx * dx + dy * dy < cr_sq) {
                    gfx_draw_pixel(x + radius - 1 - dx, y + radius - 1 - dy, color);
                    gfx_draw_pixel(x + w - radius + dx, y + radius - 1 - dy, color);
                    gfx_draw_pixel(x + radius - 1 - dx, y + h - radius + dy, color);
                    gfx_draw_pixel(x + w - radius + dx, y + h - radius + dy, color);
                    break;
                }
            }
        }
    }
}

void gfx_draw_shadow(int x, int y, int w, int h, int blur_radius, uint32_t shadow_color, int opacity)
{
    if (opacity > 255) opacity = 255;
    if (opacity < 0) opacity = 0;
    if (blur_radius <= 0) blur_radius = 1;

    uint32_t shadow_argb = (shadow_color & 0x00FFFFFF) | (((uint32_t)opacity) << 24);

    for (int i = blur_radius; i >= 1; i--) {
        uint8_t cur_opacity = (uint8_t)((opacity * (blur_radius - i + 1)) / (blur_radius * 2));
        if (cur_opacity < 5) continue;

        uint32_t cur_shadow = (shadow_color & 0x00FFFFFF) | ((uint32_t)cur_opacity << 24);

        gfx_fill_rect_blend(x + w + i - 1, y + i, 1, h - i + 1, cur_shadow);
        gfx_fill_rect_blend(x + i, y + h + i - 1, w - i + 1, 1, cur_shadow);
        gfx_draw_pixel_blend(x + w + i - 1, y + h + i - 1, cur_shadow);
    }
}

void gfx_draw_line(int x1, int y1, int x2, int y2, uint32_t color, int thickness)
{
    if (thickness <= 0) return;

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (dx > dy ? dx : dy);
    if (steps < 0) steps = -steps;

    if (steps == 0) {
        gfx_fill_rect(x1, y1, thickness, thickness, color);
        return;
    }

    for (int i = 0; i <= steps; i++) {
        int x = x1 + (dx * i) / steps;
        int y = y1 + (dy * i) / steps;
        gfx_fill_rect(x, y, thickness, thickness, color);
    }
}

void gfx_fill_circle(int cx, int cy, int radius, uint32_t color)
{
    int r_sq = radius * radius;
    for (int y = -radius; y <= radius; y++) {
        int line_start = 0;
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= r_sq) {
                line_start = x;
                int line_end = x;
                while (line_end <= radius && (line_end * line_end + y * y <= r_sq)) {
                    line_end++;
                }
                gfx_fill_rect(cx + line_start, cy + y, line_end - line_start, 1, color);
                x = line_end;
            }
        }
    }
}

void gfx_stroke_circle(int cx, int cy, int radius, int thickness, uint32_t color)
{
    for (int t = 0; t < thickness; t++) {
        int r = radius - t;
        int r_sq_min = (r - 1) * (r - 1);
        int r_sq_max = r * r;

        for (int y = -r; y <= r; y++) {
            for (int x = -r; x <= r; x++) {
                int dist_sq = x * x + y * y;
                if (dist_sq >= r_sq_min && dist_sq <= r_sq_max) {
                    gfx_draw_pixel(cx + x, cy + y, color);
                }
            }
        }
    }
}

void gfx_pixel_blend(int x, int y, uint32_t color, uint8_t alpha)
{
    if (alpha == 255) {
        gfx_draw_pixel(x, y, color);
    } else if (alpha > 0) {
        uint32_t blended = (color & 0x00FFFFFF) | ((uint32_t)alpha << 24);
        gfx_draw_pixel_blend(x, y, blended);
    }
}

void gfx_draw_window(int x, int y, int w, int h, uint32_t bg_color,
                     uint32_t shadow_color, int corner_radius)
{
    gfx_draw_shadow(x, y, w, h, 8, shadow_color, 80);
    gfx_fill_rect_rounded_blend(x, y, w, h, corner_radius, bg_color);
    gfx_stroke_rect_rounded(x, y, w, h, corner_radius, 1, shadow_color);
}
