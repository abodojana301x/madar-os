/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Interrupt Descriptor Table implementation
 * Status: [ ] Testing [ ] Pass [ ] Failed
 */

#include <idt.h>
#include <types.h>
#include <compiler.h>

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_descriptor idt_desc;

extern void *interrupt_handlers[];

void idt_init(void)
{
    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base = (uint64_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0);
    }

    // Load IDT
    idt_load();
}

void idt_set_gate(uint8_t num, uint64_t handler, uint8_t type)
{
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = 0x08; // Kernel code segment
    idt[num].ist = 0;
    idt[num].type_attr = type;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].zero = 0;
}

void idt_load(void)
{
    asm volatile("lidt %0" : : "m"(idt_desc));
}