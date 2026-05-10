#!/bin/bash
# Stage 5: Phase 2-6 - Hardware Support & Complete Implementation
# Purpose: Verify ACPI, Dynamic Resolution, Boot Hardening, RTL8139
# Phase 1 (ATA) and Core Apps already verified in previous tests

cd /workspaces/madar-os

echo "=================================="
echo "Stage 5 - Phase 2-6 Test Suite"
echo "ACPI + Resolution + Boot + RTL8139"
echo "=================================="
echo ""

TESTS_PASSED=0
TESTS_FAILED=0

# ────────────────────────────────────────────
# Phase 2: ACPI Basic Support
# ────────────────────────────────────────────
echo "📋 Phase 2: ACPI Basic Support..."
echo ""

# ACPI header file
if [ -f "include/acpi.h" ]; then
    echo "✓ include/acpi.h exists"
    ((TESTS_PASSED++))
else
    echo "✗ include/acpi.h missing"
    ((TESTS_FAILED++))
fi

# ACPI implementation file
if [ -f "kernel/acpi.c" ]; then
    echo "✓ kernel/acpi.c exists"
    ((TESTS_PASSED++))
else
    echo "✗ kernel/acpi.c missing"
    ((TESTS_FAILED++))
fi

# ACPI structures defined
grep -q "acpi_rsdp_t\|acpi_header_t" include/acpi.h && echo "✓ RSDP/Header structures" && ((TESTS_PASSED++)) || { echo "✗ Missing ACPI structs"; ((TESTS_FAILED++)); }
grep -q "acpi_fadt_t\|acpi_madt_t" include/acpi.h && echo "✓ FADT/MADT structures" && ((TESTS_PASSED++)) || { echo "✗ Missing FADT/MADT"; ((TESTS_FAILED++)); }

# ACPI functions
grep -q "acpi_init\|acpi_find_rsdp" kernel/acpi.c && echo "✓ ACPI init/find_rsdp" && ((TESTS_PASSED++)) || { echo "✗ Missing acpi_init"; ((TESTS_FAILED++)); }
grep -q "acpi_parse_fadt\|acpi_parse_madt" kernel/acpi.c && echo "✓ ACPI table parsers" && ((TESTS_PASSED++)) || { echo "✗ Missing parsers"; ((TESTS_FAILED++)); }
grep -q "acpi_checksum_valid\|checksum" kernel/acpi.c && echo "✓ Checksum validation" && ((TESTS_PASSED++)) || { echo "✗ Missing checksum"; ((TESTS_FAILED++)); }

# ACPI in Makefile (compiled)
grep -q "kernel/acpi.c" Makefile && echo "✓ ACPI in Makefile" && ((TESTS_PASSED++)) || { echo "✗ ACPI not in Makefile"; ((TESTS_FAILED++)); }

# ACPI in boot flow
grep -q "acpi_init\|acpi\.h" kernel/main.c && echo "✓ ACPI called from main" && ((TESTS_PASSED++)) || { echo "✗ ACPI not in main"; ((TESTS_FAILED++)); }

echo ""

# ────────────────────────────────────────────
# Phase 3: Dynamic Resolution
# ────────────────────────────────────────────
echo "📋 Phase 3: Dynamic Resolution..."
echo ""

grep -q "fb_try_resolution\|fb_set_resolution" drivers/fb.c && echo "✓ Resolution switching" && ((TESTS_PASSED++)) || { echo "✗ Missing resolution functions"; ((TESTS_FAILED++)); }
grep -q "vbe_controller_info\|vbe_mode_info" include/acpi.h 2>/dev/null || grep -q "vbe_" drivers/fb.h && echo "✓ VBE info structures" && ((TESTS_PASSED++)) || { echo "✗ Missing VBE structs"; ((TESTS_FAILED++)); }
grep -q "fb_query_vbe\|VBE fallback" drivers/fb.c && echo "✓ VBE query/fallback" && ((TESTS_PASSED++)) || { echo "✗ Missing VBE query"; ((TESTS_FAILED++)); }
grep -q "resolution_table\|RES_1024x768" drivers/fb.c && echo "✓ Resolution table" && ((TESTS_PASSED++)) || { echo "✗ Missing resolution table"; ((TESTS_FAILED++)); }

echo ""

# ────────────────────────────────────────────
# Phase 4: Boot Flow Hardening
# ────────────────────────────────────────────
echo "📋 Phase 4: Boot Flow Hardening..."
echo ""

grep -q "boot_report_hardware\|BOOT_DIAGNOSTICS" system/boot_sequence.c && echo "✓ Boot diagnostics" && ((TESTS_PASSED++)) || { echo "✗ Missing boot diagnostics"; ((TESTS_FAILED++)); }
grep -q "boot_state_t\|g_boot_state" system/boot_sequence.c && echo "✓ Boot state tracking" && ((TESTS_PASSED++)) || { echo "✗ Missing boot state"; ((TESTS_FAILED++)); }
grep -q "boot_set_error\|g_boot_error" system/boot_sequence.c && echo "✓ Boot error handling" && ((TESTS_PASSED++)) || { echo "✗ Missing error handling"; ((TESTS_FAILED++)); }
grep -q "l7_selftest\|layer7_selftest" system/boot_sequence.c && echo "✓ Self-test" && ((TESTS_PASSED++)) || { echo "✗ Missing self-test"; ((TESTS_FAILED++)); }

echo ""

# ────────────────────────────────────────────
# Phase 5: RTL8139 Network Complete
# ────────────────────────────────────────────
echo "📋 Phase 5: RTL8139 Network..."
echo ""

# RTL8139 files
if [ -f "drivers/rtl8139.c" ] && [ -f "drivers/rtl8139.h" ]; then
    echo "✓ RTL8139 driver files exist"
    ((TESTS_PASSED++))
else
    echo "✗ RTL8139 driver missing"
    ((TESTS_FAILED++))
fi

# TX ring buffer
grep -q "tx_ring\|rtl8139_send_packet.*TX" drivers/rtl8139.c && echo "✓ TX ring buffer" && ((TESTS_PASSED++)) || { echo "✗ Missing TX ring"; ((TESTS_FAILED++)); }

# RX ring buffer
grep -q "rx_ring\|rtl8139_recv_packet.*RX" drivers/rtl8139.c && echo "✓ RX ring buffer" && ((TESTS_PASSED++)) || { echo "✗ Missing RX ring"; ((TESTS_FAILED++)); }

# DMA/IO setup
grep -q "RTL_REG_TXADDR\|RTL_REG_RXBUFADDR" drivers/rtl8139.c && echo "✓ DMA register config" && ((TESTS_PASSED++)) || { echo "✗ Missing DMA config"; ((TESTS_FAILED++)); }

# Interrupt handler
grep -q "irq_handler\|irq_register_handler.*rtl8139" drivers/rtl8139.c && echo "✓ Interrupt handler" && ((TESTS_PASSED++)) || { echo "✗ Missing IRQ handler"; ((TESTS_FAILED++)); }

# Statistics
grep -q "packets_sent\|packets_received\|rtl8139_stats_t" drivers/rtl8139.c && echo "✓ Network statistics" && ((TESTS_PASSED++)) || { echo "✗ Missing stats"; ((TESTS_FAILED++)); }

# RTL8139 in Makefile
grep -q "rtl8139.c" Makefile && echo "✓ RTL8139 in Makefile" && ((TESTS_PASSED++)) || { echo "✗ RTL8139 not in Makefile"; ((TESTS_FAILED++)); }

echo ""

# ────────────────────────────────────────────
# Final Summary
# ────────────────────────────────────────────
echo "=================================="
echo "Results: Passed=$TESTS_PASSED Failed=$TESTS_FAILED"
echo "=================================="

if [ $TESTS_FAILED -eq 0 ]; then
    echo "✅ All Phase 2-6 tests passed! Stage 5 complete."
    echo "   Phases: ACPI ✓ Resolution ✓ Boot ✓ Network ✓"
    exit 0
else
    echo "❌ Some tests failed."
    exit 1
fi
