/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: Programmable Interrupt Controller management
 * Constraints: Legacy PIC, will be replaced in later layers
 */

#ifndef PIC_H
#define PIC_H

#include <types.h>

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// PIC commands
#define PIC_EOI 0x20
#define PIC_INIT 0x11

// Function prototypes
void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);

#endif