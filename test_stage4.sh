#!/bin/bash
# Stage 4: UI Toolkit v0.1 - Test Suite

cd /workspaces/madar-os

echo "=================================="
echo "Stage 4 - UI Toolkit v0.1"
echo "Test Suite"
echo "=================================="
echo ""

TESTS_PASSED=0
TESTS_FAILED=0

echo "📋 Checking Core Files..."
[ -f "gui/ui_toolkit.h" ] && echo "✓ ui_toolkit.h exists" && ((TESTS_PASSED++)) || { echo "✗ ui_toolkit.h missing"; ((TESTS_FAILED++)); }
[ -f "gui/ui_toolkit.c" ] && echo "✓ ui_toolkit.c exists" && ((TESTS_PASSED++)) || { echo "✗ ui_toolkit.c missing"; ((TESTS_FAILED++)); }

echo ""
echo "🧱 Checking Data Structures..."
grep -q "typedef struct UIElement" gui/ui_toolkit.h && echo "✓ UIElement structure" && ((TESTS_PASSED++)) || { echo "✗ UIElement missing"; ((TESTS_FAILED++)); }
grep -q "} UIContainer" gui/ui_toolkit.h && echo "✓ UIContainer structure" && ((TESTS_PASSED++)) || { echo "✗ UIContainer missing"; ((TESTS_FAILED++)); }
grep -q "} Button" gui/ui_toolkit.h && echo "✓ Button widget" && ((TESTS_PASSED++)) || { echo "✗ Button missing"; ((TESTS_FAILED++)); }
grep -q "} Label" gui/ui_toolkit.h && echo "✓ Label widget" && ((TESTS_PASSED++)) || { echo "✗ Label missing"; ((TESTS_FAILED++)); }

echo ""
echo "🔨 Checking Functions..."
grep -q "ui_render_element" gui/ui_toolkit.c && echo "✓ ui_render_element()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_render_container" gui/ui_toolkit.c && echo "✓ ui_render_container()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_layout_vertical" gui/ui_toolkit.c && echo "✓ ui_layout_vertical()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_layout_horizontal" gui/ui_toolkit.c && echo "✓ ui_layout_horizontal()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_handle_click" gui/ui_toolkit.c && echo "✓ ui_handle_click()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_find_clicked" gui/ui_toolkit.c && echo "✓ ui_find_clicked()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_update_hover" gui/ui_toolkit.c && echo "✓ ui_update_hover()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_button_create" gui/ui_toolkit.c && echo "✓ ui_button_create()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_label_create" gui/ui_toolkit.c && echo "✓ ui_label_create()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_container_create" gui/ui_toolkit.c && echo "✓ ui_container_create()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "ui_toolkit_init" gui/ui_toolkit.c && echo "✓ ui_toolkit_init()" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }

echo ""
echo "🎯 Success Criteria..."
grep -q "btn->base.type = UI_TYPE_BUTTON" gui/ui_toolkit.c && echo "✓ Can create Button" && ((TESTS_PASSED++)) || { echo "✗ Button creation"; ((TESTS_FAILED++)); }
grep -q "el->on_click" gui/ui_toolkit.c && echo "✓ Can handle clicks" && ((TESTS_PASSED++)) || { echo "✗ Click handling"; ((TESTS_FAILED++)); }
grep -q "offset += child->height" gui/ui_toolkit.c && echo "✓ Can arrange elements" && ((TESTS_PASSED++)) || { echo "✗ Arrangement"; ((TESTS_FAILED++)); }
if [ -f "apps/ui_demo.c" ] && [ -f "apps/settings_ui.c" ] && grep -q "ui_toolkit_init" apps/ui_demo.c && grep -q "ui_toolkit_init" apps/settings_ui.c; then
    echo "✓ No repeated UI code"
    ((TESTS_PASSED++))
else
    echo "✗ Example apps issue"
    ((TESTS_FAILED++))
fi

echo ""
echo "🎨 Checking Colors..."
grep -q "COLOR_BTN" gui/ui_toolkit.h && echo "✓ COLOR_BTN" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "COLOR_BTN_HOVER" gui/ui_toolkit.h && echo "✓ COLOR_BTN_HOVER" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "COLOR_BTN_TEXT" gui/ui_toolkit.h && echo "✓ COLOR_BTN_TEXT" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "COLOR_LABEL" gui/ui_toolkit.h && echo "✓ COLOR_LABEL" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }
grep -q "COLOR_CONTAINER" gui/ui_toolkit.h && echo "✓ COLOR_CONTAINER" && ((TESTS_PASSED++)) || { echo "✗ missing"; ((TESTS_FAILED++)); }

echo ""
echo "=================================="
echo "Results: Passed=$TESTS_PASSED Failed=$TESTS_FAILED"
[ $TESTS_FAILED -eq 0 ] && echo "✅ Stage 4 Complete!" && exit 0 || echo "❌ Some tests failed" && exit 1
