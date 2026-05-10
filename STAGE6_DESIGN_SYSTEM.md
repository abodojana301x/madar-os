# Stage 6: Design System & Polish Guide

## 🎨 Design Philosophy

**Principle**: Consistency, Speed, Clarity

All UI elements must follow this hierarchy:
1. **Consistency** - Every element must feel part of the same system
2. **Performance** - No element should cause lag or delay
3. **Clarity** - Users always know what to do

---

## 🎨 Color System

### Dark Theme (Default)
```
Background:     #1E1E1E (Primary)
Surface:        #333333 (Panel/Window)
Text:           #FFFFFF (Primary)
Accent:         #4CAF50 (Green)
```

### Light Theme
```
Background:     #F5F5F5 (Primary)
Surface:        #FFFFFF (Panel/Window)
Text:           #0A0E27 (Primary)
Accent:         #2196F3 (Blue)
```

---

## 🪟 Window Style

### Properties
- **Rounded Corners**: 8px radius
- **Shadow**: 4px blur, 2px offset
- **Title Bar**: 24px height
- **Border**: 1px, subtle

### States
- **Focused**: Accent color titlebar
- **Unfocused**: Dimmer titlebar
- **Close Button**: Top-right, red accent

### Example (C)
```c
gfx_draw_window(x, y, width, height, 
                theme->colors.window_bg,
                theme->colors.window_shadow,
                8); /* radius */
```

---

## 📊 Taskbar Design

- **Height**: 48px
- **Icons**: 32x32px with padding
- **Icon Spacing**: 8px between
- **Active Indicator**: 3px bar at bottom
- **Hover Effect**: Color change + scale

---

## 🔘 Button States

```
Normal:   [gray background]
Hover:    [lighter background] + [1px border]
Active:   [accent color] + [scale 0.95]
Focused:  [2px accent border]
Disabled: [very light] + [dim text]
```

### Rendering
```c
uint32_t color = theme_get_button_color(state);
gfx_fill_rect_rounded(x, y, w, h, radius, color);
font_draw_text(x + 8, y + 8, label, text_color, 0);
```

---

## ✏️ Typography

### Font Sizes
- Title Bar: 12px
- Button: 12px
- Body: 11px
- Small: 10px

### RTL Support
- Arabic text: Right-aligned
- Mixed text: Directional awareness
- Cursor: Position-aware

### Implementation
```c
rtl_draw_text_right_aligned(x, y, "مرحبا", color);
```

---

## ✨ Micro-Interactions

### Rules
- ✅ Duration < 150ms
- ✅ Smooth easing (ease-out)
- ✅ Feedback immediate
- ❌ No heavy animations
- ❌ No frame drops

### Common
1. **Button Click**: Slight scale + color change
2. **Hover**: Color fade (50ms)
3. **Window Focus**: Titlebar color change (100ms)
4. **Menu Open**: Fade in (100ms)

---

## 🎯 Spacing System

```
Small:   4px   (internal padding)
Medium:  8px   (element spacing)
Large:   16px  (section spacing)

Title Height:  24px
Taskbar Height: 48px
Button Height: 32px
```

---

## ✅ Quality Checklist

Before marking Stage 6 complete:

- [ ] All windows have rounded corners
- [ ] All windows have proper shadows
- [ ] Taskbar icons are properly spaced
- [ ] Buttons have hover states
- [ ] All text is readable
- [ ] Arabic text displays correctly
- [ ] No animations exceed 150ms
- [ ] No visible lag or stutter
- [ ] Theme colors are consistent
- [ ] Active window titlebar stands out
- [ ] Close button is clearly visible
- [ ] Disabled elements are clearly dimmed

---

## 📁 File Structure

```
gui/
├── theme.h / theme.c           ← Color system
├── gfx_utils.h / gfx_utils.c   ← Rounded corners, shadows
├── animation.h / animation.c   ← Micro-interactions
├── rtl_support.h / rtl_support.c ← Arabic support
├── widgets_modern.h / widgets_modern.c ← State system
├── wm_core.c                   ← Modern window rendering
├── taskbar.c                   ← Modern taskbar
└── ui_toolkit.h                ← Base widget system
```

---

## 🧪 Testing

Run tests to verify:
```bash
make test_stage6
```

Should verify:
- Theme colors are applied
- Windows render with modern style
- Taskbar is properly laid out
- Buttons respond to hover
- RTL text is handled
- Animations complete < 150ms

---

## 🚀 Performance Targets

- Window render: < 2ms
- Taskbar render: < 1ms
- Event response: < 10ms
- Animation frame: 60fps (16ms/frame)

---

## 📝 Implementation Checklist

- [x] Theme System (Light/Dark)
- [x] Graphics Utilities (rounded, shadow)
- [x] Modern Widgets with state
- [x] Window Manager integration
- [x] Taskbar redesign
- [x] RTL text support
- [x] Animation framework
- [ ] Integration testing
- [ ] Performance validation
- [ ] Polish refinement

