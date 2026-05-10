#ifndef GUI_THEME_H
#define GUI_THEME_H

#include <types.h>

/* UI States */
typedef enum {
    UI_STATE_NORMAL = 0,
    UI_STATE_HOVER = 1,
    UI_STATE_ACTIVE = 2,
    UI_STATE_FOCUSED = 3,
    UI_STATE_DISABLED = 4
} ui_state_t;

/* Color palette */
typedef struct {
    /* Primary colors */
    uint32_t bg_primary;          /* Main background */
    uint32_t bg_secondary;        /* Secondary background */
    uint32_t bg_surface;          /* Surface/panel background */
    
    /* Text colors */
    uint32_t text_primary;        /* Main text */
    uint32_t text_secondary;      /* Secondary text */
    uint32_t text_disabled;       /* Disabled text */
    
    /* Accent and states */
    uint32_t accent;              /* Primary accent */
    uint32_t accent_light;        /* Light accent */
    uint32_t accent_dark;         /* Dark accent */
    
    /* UI Elements */
    uint32_t button_normal;       /* Button normal state */
    uint32_t button_hover;        /* Button hover state */
    uint32_t button_active;       /* Button active state */
    uint32_t button_disabled;     /* Button disabled state */
    
    uint32_t window_bg;           /* Window background */
    uint32_t window_titlebar;     /* Window titlebar */
    uint32_t window_titlebar_active;  /* Active window titlebar */
    uint32_t window_titlebar_text;    /* Titlebar text */
    uint32_t window_border;       /* Window border */
    uint32_t window_shadow;       /* Window shadow */
    
    /* Taskbar */
    uint32_t taskbar_bg;          /* Taskbar background */
    uint32_t taskbar_icon_normal; /* Taskbar icon normal */
    uint32_t taskbar_icon_hover;  /* Taskbar icon hover */
    uint32_t taskbar_icon_active; /* Taskbar icon active */
    uint32_t taskbar_indicator;   /* Active indicator */
    
    /* Other */
    uint32_t border_light;        /* Light borders */
    uint32_t border_dark;         /* Dark borders */
    uint32_t warning;             /* Warning color */
    uint32_t error;               /* Error color */
    uint32_t success;             /* Success color */
} theme_palette_t;

/* Spacing and sizing constants */
typedef struct {
    int radius;                   /* Corner radius (8px for modern look) */
    int shadow_blur;              /* Shadow blur radius */
    int shadow_offset_x;          /* Shadow offset X */
    int shadow_offset_y;          /* Shadow offset Y */
    int padding_small;            /* 4px */
    int padding_medium;           /* 8px */
    int padding_large;            /* 16px */
    int border_width;             /* Border width */
    int title_height;             /* Window title bar height */
    int taskbar_height;           /* Taskbar height */
    int button_height;            /* Standard button height */
} theme_spacing_t;

/* Full theme structure */
typedef struct {
    const char *name;             /* Theme name ("light", "dark") */
    int is_dark;                  /* 1 for dark, 0 for light */
    theme_palette_t colors;
    theme_spacing_t spacing;
} theme_t;

/* Global theme management */
extern theme_t *g_current_theme;

/* Theme initialization */
void theme_init(void);
void theme_set_dark(void);
void theme_set_light(void);
theme_t *theme_get(void);
theme_t *theme_get_dark(void);
theme_t *theme_get_light(void);

/* Color helpers */
uint32_t theme_get_color(const char *name);
uint32_t theme_blend(uint32_t color1, uint32_t color2, int blend_percent);
uint32_t theme_darken(uint32_t color, int amount);
uint32_t theme_lighten(uint32_t color, int amount);

/* State-based color selection */
uint32_t theme_get_button_color(ui_state_t state);
uint32_t theme_get_text_color(ui_state_t state);
uint32_t theme_get_window_titlebar_color(int is_focused);

#endif
