/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Interrupt Descriptor Table management
 * Constraints: x86_64 only
 */

#ifndef IDT_H
#define IDT_H

#include <types.h>
#include <compiler.h>

// IDT entry structure
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} PACKED_ATTR;

// IDT descriptor
struct idt_descriptor {
    uint16_t limit;
    uint64_t base;
} PACKED_ATTR;

#define IDT_ENTRIES 256

// Interrupt types
#define IDT_TYPE_INTERRUPT 0x8E
#define IDT_TYPE_TRAP 0x8F

// Function prototypes
void idt_init(void);
void idt_set_gate(uint8_t num, uint64_t handler, uint8_t type);
void idt_load(void);

#endif