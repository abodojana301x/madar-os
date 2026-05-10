/*
 * Layer: 2 (Kernel Core) - Timer Management
 * File: timer.h
 * Purpose: PIT timer setup for scheduling
 * Status: New for Layer 2
 */

#ifndef TIMER_H
#define TIMER_H

#include <types.h>

/* PIT Configuration */
#define PIT_FREQUENCY       1193182    /* PIT oscillator frequency */
#define TIMER_FREQUENCY     100        /* 100 Hz = 10ms ticks */
#define PIT_DIVISOR         (PIT_FREQUENCY / TIMER_FREQUENCY)

/* PIT Ports */
#define PIT_COMMAND_PORT    0x43
#define PIT_CHANNEL0_PORT   0x40

/* PIT Commands */
#define PIT_CHANNEL0        0x00
#define PIT_ACCESS_LOHI     0x30
#define PIT_MODE2           0x04    /* Rate Generator */

/* Function Prototypes */
void timer_init(void);
void timer_handler(void);
uint64_t timer_get_ticks(void);

#endif /* TIMER_H */