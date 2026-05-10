/*
 * Layer: 3 (Drivers & Hardware Abstraction) - IRQ Manager
 * File: irq_manager.h
 * Purpose: Centralized IRQ/interrupt management
 * Status: New for Layer 3
 */

#ifndef IRQ_MANAGER_H
#define IRQ_MANAGER_H

#include <types.h>

/* IRQ Numbers */
#define IRQ_TIMER           0
#define IRQ_KEYBOARD        1
#define IRQ_CASCADE         2
#define IRQ_COM2            3
#define IRQ_COM1            4
#define IRQ_LPT2            5
#define IRQ_FLOPPY          6
#define IRQ_LPT1            7
#define IRQ_RTC             8
#define IRQ_NETWORK         9
#define IRQ_RESERVE0        10
#define IRQ_RESERVE1        11
#define IRQ_MOUSE           12
#define IRQ_FPU             13
#define IRQ_ATA_PRIMARY     14
#define IRQ_ATA_SECONDARY   15

/* IRQ Handler Callback */
typedef void (*irq_handler_t)(void);

/* Function Prototypes */
void irq_manager_init(void);
void irq_register_handler(uint8_t irq, irq_handler_t handler);
void irq_unregister_handler(uint8_t irq);
void irq_dispatch(uint8_t irq);
void irq_enable(uint8_t irq);
void irq_disable(uint8_t irq);

#endif /* IRQ_MANAGER_H */
