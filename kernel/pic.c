/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Programmable Interrupt Controller implementation
 * Status: [ ] Testing [ ] Pass [ ] Failed
 */

#include <pic.h>
#include <types.h>

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void pic_init(void)
{
    // ICW1: Start initialization
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // ICW2: Set interrupt vectors
    outb(PIC1_DATA, 0x20); // PIC1 interrupts start at 32
    outb(PIC2_DATA, 0x28); // PIC2 interrupts start at 40

    // ICW3: Tell PICs about each other
    outb(PIC1_DATA, 0x04); // PIC1 has slave at IRQ2
    outb(PIC2_DATA, 0x02); // PIC2 is slave

    // ICW4: Set 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Mask all interrupts initially
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_mask_irq(uint8_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port) | (1 << (irq % 8));
    outb(port, mask);
}

void pic_unmask_irq(uint8_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port) & ~(1 << (irq % 8));
    outb(port, mask);
}