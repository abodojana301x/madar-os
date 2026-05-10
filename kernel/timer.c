/*
 * Layer: 2 (Kernel Core) - Timer Implementation
 * File: timer.c
 * Purpose: PIT timer setup and interrupt handling
 * Status: New for Layer 2
 */

#include <timer.h>
#include <pic.h>
#include <idt.h>
#include <kprintf.h>
#include <io.h>

/* Global tick counter */
static volatile uint64_t timer_ticks = 0;

/*
 * Initialize PIT timer
 */
void timer_init(void)
{
    kprintf("⏰ Initializing PIT Timer...\n");
    kprintf("  Frequency: %d Hz (%d ms ticks)\n", TIMER_FREQUENCY, 1000 / TIMER_FREQUENCY);

    /* Set PIT divisor */
    uint16_t divisor = PIT_DIVISOR;

    /* Send command byte */
    outb(PIT_COMMAND_PORT, PIT_CHANNEL0 | PIT_ACCESS_LOHI | PIT_MODE2);

    /* Send divisor (low byte then high byte) */
    outb(PIT_CHANNEL0_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);

    /* Register timer interrupt handler */
    idt_set_gate(32, (uint64_t)timer_handler, IDT_TYPE_INTERRUPT);  /* IRQ0 = interrupt 32 */

    kprintf("✅ PIT timer initialized\n");
}

/*
 * Timer interrupt handler
 */
void timer_handler(void)
{
    timer_ticks++;

    /* Call scheduler tick */
    extern void scheduler_tick(void);
    scheduler_tick();

    /* Send EOI to PIC */
    pic_send_eoi(0);
}

/*
 * Get current tick count
 */
uint64_t timer_get_ticks(void)
{
    return timer_ticks;
}