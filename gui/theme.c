#include "theme.h"
#include <string.h>

theme_t *g_current_theme = NULL;

/* Dark theme palette - Windows 11 Dark Mode */
static theme_t g_theme_dark = {
    .name = "dark",
    .is_dark = 1,
    .colors = {
        /* Primary colors - Windows 11 Dark */
        .bg_primary = 0x001C1C1C,      /* Dark background */
        .bg_secondary = 0x00262626,    /* Secondary darker */
        .bg_surface = 0x002D2D2D,      /* Surface */
        
        /* Text colors */
        .text_primary = 0x00E7E7E7,    /* Light gray text (readable) */
        .text_secondary = 0x00ABABAB,  /* Medium gray text */
        .text_disabled = 0x00666666,   /* Dim gray text */
        
        /* Accent colors - Windows 11 Blue Acrylic */
        .accent = 0x000078D4,          /* Windows Blue */
        .accent_light = 0x005BA3E8,    /* Light blue */
        .accent_dark = 0x00004A90E2,   /* Dark blue */
        
        /* Button states */
        .button_normal = 0x003D3D3D,   /* Dark gray button */
        .button_hover = 0x004D4D4D,    /* Hover gray */
        .button_active = 0x000078D4,   /* Blue active */
        .button_disabled = 0x00262626, /* Very dark gray */
        
        /* Window elements */
        .window_bg = 0x001E1E1E,       /* Dark window */
        .window_titlebar = 0x00262626, /* Inactive titlebar */
        .window_titlebar_active = 0x001E1E1E, /* Active titlebar */
        .window_titlebar_text = 0x00E7E7E7,   /* Titlebar text */
        .window_border = 0x00404040,   /* Subtle border */
        .window_shadow = 0x00000000,   /* Black shadow */
        
        /* Taskbar */
        .taskbar_bg = 0x00151515,      /* Very dark taskbar */
        .taskbar_icon_normal = 0x00A0A0A0,
        .taskbar_icon_hover = 0x00E7E7E7,
        .taskbar_icon_active = 0x000078D4,
        .taskbar_indicator = 0x000078D4,
        
        /* Other */
        .border_light = 0x00404040,
        .border_dark = 0x00000000,
        .warning = 0x00FFB900,         /* Amber */
        .error = 0x00E74856,           /* Red */
        .success = 0x0010893E,         /* Green */
    },
    .spacing = {
        .radius = 12,                  /* 12px rounded corners (Windows 11) */
        .shadow_blur = 8,
        .shadow_offset_x = 0,
        .shadow_offset_y = 2,
        .padding_small = 4,
        .padding_medium = 8,
        .padding_large = 12,
        .border_width = 1,
        .title_height = 32,            /* Larger titlebar */
        .taskbar_height = 48,
        .button_height = 32,
    }
};

/* Light theme palette - Windows 11 Light Mode */
static theme_t g_theme_light = {
    .name = "light",
    .is_dark = 0,
    .colors = {
        /* Primary colors - Windows 11 Light */
        .bg_primary = 0x00FFFFFF,      /* White background */
        .bg_secondary = 0x00F9F9F9,    /* Off-white secondary */
        .bg_surface = 0x00F8F8F8,      /* Subtle surface */
        
        /* Text colors */
        .text_primary = 0x00202020,    /* Dark text */
        .text_secondary = 0x00717171,  /* Medium gray text */
        .text_disabled = 0x00AAAAAA,   /* Light gray text */
        
        /* Accent colors - Windows 11 Blue */
        .accent = 0x000078D4,          /* Windows Blue */
        .accent_light = 0x005BA3E8,    /* Light blue */
        .accent_dark = 0x00004A90E2,   /* Dark blue */
        
        /* Button states */
        .button_normal = 0x00E8E8E8,   /* Light gray button */
        .button_hover = 0x00D8D8D8,    /* Hover gray */
        .button_active = 0x000078D4,   /* Blue active */
        .button_disabled = 0x00F2F2F2, /* Very light gray */
        
        /* Window elements */
        .window_bg = 0x00FFFFFF,       /* White window */
        .window_titlebar = 0x00F6F6F6, /* Inactive titlebar */
        .window_titlebar_active = 0x00FFFFFF, /* Active titlebar */
        .window_titlebar_text = 0x00202020,   /* Titlebar text */
        .window_border = 0x00D9D9D9,   /* Subtle border */
        .window_shadow = 0x00000000,   /* Black shadow */
        
        /* Taskbar */
        .taskbar_bg = 0x00F9F9F9,      /* Very light taskbar */
        .taskbar_icon_normal = 0x00717171,
        .taskbar_icon_hover = 0x00202020,
        .taskbar_icon_active = 0x000078D4,
        .taskbar_indicator = 0x000078D4,
        
        /* Other */
        .border_light = 0x00E0E0E0,
        .border_dark = 0x00999999,
        .warning = 0x00FFB900,         /* Amber */
        .error = 0x00E74856,           /* Red */
        .success = 0x0010893E,         /* Green */
    },
    .spacing = {
        .radius = 12,                  /* 12px rounded corners (Windows 11) */
        .shadow_blur = 8,
        .shadow_offset_x = 0,
        .shadow_offset_y = 2,
        .padding_small = 4,
        .padding_medium = 8,
        .padding_large = 12,
        .border_width = 1,
        .title_height = 32,            /* Larger titlebar */
        .taskbar_height = 48,
        .button_height = 32,
    }
};

void theme_init(void)
{
    g_current_theme = &g_theme_dark;
}

void theme_set_dark(void)
{
    g_current_theme = &g_theme_dark;
}

void theme_set_light(void)
{
    g_current_theme = &g_theme_light;
}

theme_t *theme_get(void)
{
    if (!g_current_theme) theme_init();
    return g_current_theme;
}

theme_t *theme_get_dark(void)
{
    return &g_theme_dark;
}

theme_t *theme_get_light(void)
{
    return &g_theme_light;
}

uint32_t theme_get_color(const char *name)
{
    if (!g_current_theme) theme_init();
    if (!name) return g_current_theme->colors.text_primary;
    
    if (strcmp(name, "bg_primary") == 0) return g_current_theme->colors.bg_primary;
    if (strcmp(name, "text_primary") == 0) return g_current_theme->colors.text_primary;
    if (strcmp(name, "accent") == 0) return g_current_theme->colors.accent;
    if (strcmp(name, "button") == 0) return g_current_theme->colors.button_normal;
    
    return g_current_theme->colors.text_primary;
}

uint32_t theme_blend(uint32_t color1, uint32_t color2, int blend_percent)
{
    /* Blend two colors. blend_percent: 0 = color1, 100 = color2 */
    if (blend_percent < 0) blend_percent = 0;
    if (blend_percent > 100) blend_percent = 100;
    
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;
    
    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;
    
    uint8_t r = r1 + ((r2 - r1) * blend_percent) / 100;
    uint8_t g = g1 + ((g2 - g1) * blend_percent) / 100;
    uint8_t b = b1 + ((b2 - b1) * blend_percent) / 100;
    
    return (r << 16) | (g << 8) | b;
}

uint32_t theme_darken(uint32_t color, int amount)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    r = r > amount ? r - amount : 0;
    g = g > amount ? g - amount : 0;
    b = b > amount ? b - amount : 0;
    
    return (r << 16) | (g << 8) | b;
}

uint32_t theme_lighten(uint32_t color, int amount)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    r = r < (255 - amount) ? r + amount : 255;
    g = g < (255 - amount) ? g + amount : 255;
    b = b < (255 - amount) ? b + amount : 255;
    
    return (r << 16) | (g << 8) | b;
}

uint32_t theme_get_button_color(ui_state_t state)
{
    if (!g_current_theme) theme_init();
    
    switch (state) {
        case UI_STATE_HOVER:
            return g_current_theme->colors.button_hover;
        case UI_STATE_ACTIVE:
            return g_current_theme->colors.button_active;
        case UI_STATE_DISABLED:
            return g_current_theme->colors.button_disabled;
        case UI_STATE_FOCUSED:
            return g_current_theme->colors.accent;
        default:
            return g_current_theme->colors.button_normal;
    }
}

uint32_t theme_get_text_color(ui_state_t state)
{
    if (!g_current_theme) theme_init();
    
    switch (state) {
        case UI_STATE_DISABLED:
            return g_current_theme->colors.text_disabled;
        default:
            return g_current_theme->colors.text_primary;
    }
}

uint32_t theme_get_window_titlebar_color(int is_focused)
{
    if (!g_current_theme) theme_init();
    
    if (is_focused) {
        return g_current_theme->colors.window_titlebar_active;
    } else {
        return g_current_theme->colors.window_titlebar;
    }
}
