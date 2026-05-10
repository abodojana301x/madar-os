/*
 * Layer: 3 (Drivers & Hardware Abstraction) - IRQ Manager
 * File: irq_manager.c
 * Purpose: Centralized IRQ/interrupt management
 * Status: New for Layer 3
 */

#include "irq_manager.h"
#include "kbd.h"
#include "mouse.h"
#include "../include/kprintf.h"
#include "../include/spinlock.h"

static irq_handler_t irq_handlers[16] = {NULL};
static spinlock_t irq_lock;

/* Initialize IRQ manager */
void irq_manager_init(void) {
    kprintf("⚙️  Initializing IRQ Manager...\n");
    
    spinlock_init(&irq_lock);
    
    /* Clear all handlers */
    for (int i = 0; i < 16; i++) {
        irq_handlers[i] = NULL;
    }
    
    /* Register default handlers */
    irq_register_handler(IRQ_KEYBOARD, kbd_handler);
    irq_register_handler(IRQ_MOUSE, mouse_handler);
    
    kprintf("✅ IRQ Manager initialized\n");
}

/* Register an IRQ handler */
void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq >= 16 || !handler) {
        return;
    }
    
    spinlock_acquire(&irq_lock);
    irq_handlers[irq] = handler;
    spinlock_release(&irq_lock);
    
    kprintf("  IRQ %u handler registered\n", irq);
}

/* Unregister an IRQ handler */
void irq_unregister_handler(uint8_t irq) {
    if (irq >= 16) {
        return;
    }
    
    spinlock_acquire(&irq_lock);
    irq_handlers[irq] = NULL;
    spinlock_release(&irq_lock);
}

/* Dispatch an IRQ to its handler */
void irq_dispatch(uint8_t irq) {
    if (irq >= 16 || !irq_handlers[irq]) {
        return;
    }
    
    irq_handlers[irq]();
}

/* Enable an IRQ */
void irq_enable(uint8_t irq) {
    if (irq >= 16) {
        return;
    }
    /* Would typically unmask in PIC here */
}

/* Disable an IRQ */
void irq_disable(uint8_t irq) {
    if (irq >= 16) {
        return;
    }
    /* Would typically mask in PIC here */
}
