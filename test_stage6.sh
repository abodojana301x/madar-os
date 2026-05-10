#!/bin/bash

# Stage 6: Polish & UX Test Suite

echo "🎨 Stage 6: Polish & UX Testing"
echo "================================"
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASS=0
FAIL=0

# Test function
test_feature() {
    local name=$1
    local result=$2
    
    if [ $result -eq 0 ]; then
        echo -e "${GREEN}✅ PASS${NC}: $name"
        ((PASS++))
    else
        echo -e "${RED}❌ FAIL${NC}: $name"
        ((FAIL++))
    fi
}

# ===== TEST 1: Theme System =====
echo -e "\n${YELLOW}Testing Theme System...${NC}"

# Check if theme headers exist
if [ -f "gui/theme.h" ] && [ -f "gui/theme.c" ]; then
    test_feature "Theme files exist" 0
else
    test_feature "Theme files exist" 1
fi

# Check for dark/light theme definitions
if grep -q "g_theme_dark" gui/theme.c && grep -q "g_theme_light" gui/theme.c; then
    test_feature "Dark and light themes defined" 0
else
    test_feature "Dark and light themes defined" 1
fi

# Check for color functions
if grep -q "theme_get_button_color\|theme_get_text_color" gui/theme.c; then
    test_feature "Color helper functions" 0
else
    test_feature "Color helper functions" 1
fi


# ===== TEST 2: Graphics Utilities =====
echo -e "\n${YELLOW}Testing Graphics Utilities...${NC}"

if [ -f "gui/gfx_utils.h" ] && [ -f "gui/gfx_utils.c" ]; then
    test_feature "Graphics utility files exist" 0
else
    test_feature "Graphics utility files exist" 1
fi

# Check for rounded corner function
if grep -q "gfx_fill_rect_rounded" gui/gfx_utils.c; then
    test_feature "Rounded rectangle function" 0
else
    test_feature "Rounded rectangle function" 1
fi

# Check for shadow function
if grep -q "gfx_draw_shadow" gui/gfx_utils.c; then
    test_feature "Shadow rendering function" 0
else
    test_feature "Shadow rendering function" 1
fi


# ===== TEST 3: UI States System =====
echo -e "\n${YELLOW}Testing UI States System...${NC}"

if [ -f "gui/widgets_modern.h" ] && [ -f "gui/widgets_modern.c" ]; then
    test_feature "Modern widgets files exist" 0
else
    test_feature "Modern widgets files exist" 1
fi

# Check for state enum
if grep -q "UI_STATE_NORMAL\|UI_STATE_HOVER\|UI_STATE_ACTIVE" gui/widgets_modern.c; then
    test_feature "UI state constants defined" 0
else
    test_feature "UI state constants defined" 1
fi

# Check for button state handling
if grep -q "button_handle_mouse_over\|button_handle_click" gui/widgets_modern.c; then
    test_feature "Button state handling" 0
else
    test_feature "Button state handling" 1
fi


# ===== TEST 4: RTL Support =====
echo -e "\n${YELLOW}Testing RTL Support...${NC}"

if [ -f "gui/rtl_support.h" ] && [ -f "gui/rtl_support.c" ]; then
    test_feature "RTL support files exist" 0
else
    test_feature "RTL support files exist" 1
fi

# Check for direction detection
if grep -q "rtl_detect_direction" gui/rtl_support.c; then
    test_feature "Direction detection function" 0
else
    test_feature "Direction detection function" 1
fi

# Check for RTL text rendering
if grep -q "rtl_draw_text_right_aligned" gui/rtl_support.c; then
    test_feature "RTL text rendering" 0
else
    test_feature "RTL text rendering" 1
fi


# ===== TEST 5: Animation System =====
echo -e "\n${YELLOW}Testing Animation System...${NC}"

if [ -f "gui/animation.h" ] && [ -f "gui/animation.c" ]; then
    test_feature "Animation files exist" 0
else
    test_feature "Animation files exist" 1
fi

# Check for animation types (in header)
if grep -q "ANIM_FADE_IN\|ANIM_SLIDE_LEFT\|ANIM_SCALE" gui/animation.h; then
    test_feature "Animation types defined" 0
else
    test_feature "Animation types defined" 1
fi

# Check for easing functions
if grep -q "EASE_LINEAR\|EASE_OUT_QUAD\|EASE_IN_OUT_QUAD" gui/animation.c; then
    test_feature "Easing functions defined" 0
else
    test_feature "Easing functions defined" 1
fi


# ===== TEST 6: Window Manager Integration =====
echo -e "\n${YELLOW}Testing Window Manager Integration...${NC}"

# Check if wm_core includes theme and gfx_utils
if grep -q "#include \"theme.h\"\|#include \"gfx_utils.h\"" gui/wm_core.c; then
    test_feature "WM includes theme and graphics utilities" 0
else
    test_feature "WM includes theme and graphics utilities" 1
fi

# Check for rounded corner usage in WM
if grep -q "gfx_fill_rect_rounded" gui/wm_core.c; then
    test_feature "Windows use rounded corners" 0
else
    test_feature "Windows use rounded corners" 1
fi


# ===== TEST 7: Taskbar Update =====
echo -e "\n${YELLOW}Testing Taskbar Updates...${NC}"

# Check if taskbar includes theme
if grep -q "#include \"theme.h\"" gui/taskbar.c; then
    test_feature "Taskbar includes theme" 0
else
    test_feature "Taskbar includes theme" 1
fi

# Check for modern taskbar height
if grep -q "TASKBAR_H 48" gui/taskbar.c; then
    test_feature "Modern taskbar height (48px)" 0
else
    test_feature "Modern taskbar height (48px)" 1
fi


# ===== TEST 8: Design Documentation =====
echo -e "\n${YELLOW}Testing Documentation...${NC}"

if [ -f "STAGE6_DESIGN_SYSTEM.md" ]; then
    test_feature "Design system documentation" 0
else
    test_feature "Design system documentation" 1
fi

# Check for sections in design doc
if grep -q "Color System\|Window Style\|Button States" STAGE6_DESIGN_SYSTEM.md; then
    test_feature "Design documentation completeness" 0
else
    test_feature "Design documentation completeness" 1
fi


# ===== COMPILATION TEST =====
echo -e "\n${YELLOW}Testing Compilation...${NC}"

# Try to compile just the GUI modules
if gcc -ffreestanding -fno-stack-protector -Igui -Iinclude -c gui/theme.c -o /tmp/theme.o 2>/dev/null; then
    test_feature "Theme module compiles" 0
else
    test_feature "Theme module compiles" 1
fi

if gcc -ffreestanding -fno-stack-protector -Igui -Iinclude -c gui/gfx_utils.c -o /tmp/gfx_utils.o 2>/dev/null; then
    test_feature "Graphics utilities module compiles" 0
else
    test_feature "Graphics utilities module compiles" 1
fi

if gcc -ffreestanding -fno-stack-protector -Igui -Iinclude -c gui/rtl_support.c -o /tmp/rtl.o 2>/dev/null; then
    test_feature "RTL support module compiles" 0
else
    test_feature "RTL support module compiles" 1
fi

if gcc -ffreestanding -fno-stack-protector -Igui -Iinclude -c gui/animation.c -o /tmp/anim.o 2>/dev/null; then
    test_feature "Animation module compiles" 0
else
    test_feature "Animation module compiles" 1
fi

if gcc -ffreestanding -fno-stack-protector -Igui -Iinclude -c gui/widgets_modern.c -o /tmp/widgets.o 2>/dev/null; then
    test_feature "Modern widgets module compiles" 0
else
    test_feature "Modern widgets module compiles" 1
fi


# ===== Summary =====
echo -e "\n${YELLOW}========== TEST SUMMARY ==========${NC}"
TOTAL=$((PASS + FAIL))
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"
echo "Total:  $TOTAL"

if [ $FAIL -eq 0 ]; then
    echo -e "\n${GREEN}✅ All Stage 6 tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ Some tests failed.${NC}"
    exit 1
fi
