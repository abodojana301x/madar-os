#!/bin/bash
#
# Layer 1 Comprehensive Test Suite
# This script verifies all Layer 1 components
#

#set -e

echo "════════════════════════════════════════════════════════════"
echo "🧪 Madar OS - Layer 1 Comprehensive Test Suite"
echo "════════════════════════════════════════════════════════════"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

TEST_PASSED=0
TEST_FAILED=0

test_section() {
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "📋 $1"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

test_pass() {
    echo -e "${GREEN}✅ PASSED:${NC} $1"
    ((TEST_PASSED++))
}

test_fail() {
    echo -e "${RED}❌ FAILED:${NC} $1"
    ((TEST_FAILED++))
}

test_warn() {
    echo -e "${YELLOW}⚠️  WARNING:${NC} $1"
}

# Test 1: Build System
test_section "Build System Integrity"

if [ -f "Makefile" ]; then
    test_pass "Makefile exists"
else
    test_fail "Makefile not found"
fi

if [ -f "linker.ld" ]; then
    test_pass "linker.ld exists"
else
    test_fail "linker.ld not found"
fi

# Test 2: Header Files
test_section "Header Files"

HEADERS=(
    "include/types.h"
    "include/compiler.h"
    "include/kprintf.h"
    "include/vga.h"
    "include/serial.h"
    "include/memory.h"
    "include/idt.h"
    "include/pic.h"
    "include/multiboot2.h"
)

for header in "${HEADERS[@]}"; do
    if [ -f "$header" ]; then
        test_pass "Found: $header"
    else
        test_fail "Missing: $header"
    fi
done

# Test 3: Kernel Source Files
test_section "Kernel Source Files"

SOURCES=(
    "kernel/boot.s"
    "kernel/main.c"
    "kernel/memory.c"
    "kernel/vga.c"
    "kernel/serial.c"
    "kernel/kprintf.c"
    "kernel/idt.c"
    "kernel/pic.c"
)

for source in "${SOURCES[@]}"; do
    if [ -f "$source" ]; then
        test_pass "Found: $source"
    else
        test_fail "Missing: $source"
    fi
done

# Test 4: Clean Build
test_section "Clean Build Test"

make clean >/dev/null 2>&1
if [ $? -eq 0 ]; then
    test_pass "Make clean successful"
else
    test_fail "Make clean failed"
fi

# Test 5: Compilation
test_section "Compilation Test"

if make >/dev/null 2>&1; then
    test_pass "Compilation successful"
else
    test_fail "Compilation failed"
fi

# Test 6: Binary Size
test_section "Binary Size Verification"

if [ -f "kernel.bin" ]; then
    SIZE=$(stat -c%s "kernel.bin")
    SIZE_KB=$((SIZE / 1024))
    
    echo "Binary size: $SIZE bytes ($SIZE_KB KB)"
    
    if [ $SIZE -lt 51200 ]; then
        test_pass "Binary size under 50KB limit ($SIZE_KB KB < 50 KB)"
    else
        test_fail "Binary size exceeds 50KB limit ($SIZE_KB KB >= 50 KB)"
    fi
else
    test_fail "kernel.bin not found"
fi

# Test 7: Memory Map
test_section "Memory Map Verification"

if grep -q "ENTRY(boot_start)" linker.ld; then
    test_pass "Boot entry point defined"
else
    test_fail "Boot entry point not found"
fi

if grep -q ". = 1M" linker.ld; then
    test_pass "Kernel load address is 1MB"
else
    test_warn "Kernel load address might not be 1MB"
fi

# Test 8: Multiboot2 Header
test_section "Multiboot2 Compliance"

if grep -q "0x36d76289" kernel/boot.s; then
    test_pass "Multiboot2 magic number present"
else
    test_fail "Multiboot2 magic number not found"
fi

# Test 9: Symbol Verification
test_section "Symbol Verification"

if nm kernel.bin 2>/dev/null | grep -q "boot_start"; then
    test_pass "boot_start symbol found"
else
    test_warn "boot_start symbol not found in binary"
fi

if nm kernel.bin 2>/dev/null | grep -q "kernel_main"; then
    test_pass "kernel_main symbol found"
else
    test_fail "kernel_main symbol not found"
fi

# Test 10: Documentation
test_section "Documentation"

if [ -f "README_AR.md" ]; then
    test_pass "README_AR.md exists"
else
    test_fail "README_AR.md not found"
fi

if [ -f "LAYER1.md" ]; then
    test_pass "LAYER1.md exists"
else
    test_fail "LAYER1.md not found"
fi

if [ -f "IMPLEMENTATION_SUMMARY.md" ]; then
    test_pass "IMPLEMENTATION_SUMMARY.md exists"
else
    test_fail "IMPLEMENTATION_SUMMARY.md not found"
fi

# Final Summary
echo ""
echo "════════════════════════════════════════════════════════════"
echo "📊 Test Summary"
echo "════════════════════════════════════════════════════════════"
echo "Tests Passed: ${GREEN}${TEST_PASSED}${NC}"
echo "Tests Failed: ${RED}${TEST_FAILED}${NC}"
echo ""

if [ $TEST_FAILED -eq 0 ]; then
    echo -e "${GREEN}✅ All tests passed!${NC}"
    echo "🎉 Layer 1 is ready for deployment!"
    exit 0
else
    echo -e "${RED}❌ Some tests failed!${NC}"
    echo "⚠️  Please fix the issues above."
    exit 1
fi
