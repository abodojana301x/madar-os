/*
 * Layer: 2 (Kernel Core) - Layer 2 Report
 * File: layer2_report.c
 * Purpose: Generate comprehensive Layer 2 status report
 * Status: New for Layer 2
 */

#include <types.h>
#include <kprintf.h>
#include <heap.h>
#include <timer.h>

/* Statistics counters */
static uint32_t thread_switches = 0;
static uint32_t syscalls_executed = 0;
static uint32_t allocations_total = 0;
static uint32_t frees_total = 0;

/*
 * Increment thread switch counter
 */
void report_thread_switch(void)
{
    thread_switches++;
}

/*
 * Increment syscall counter
 */
void report_syscall(void)
{
    syscalls_executed++;
}

/*
 * Increment allocation counter
 */
void report_allocation(void)
{
    allocations_total++;
}

/*
 * Increment free counter
 */
void report_free(void)
{
    frees_total++;
}

/*
 * Generate Layer 2 status report
 */
void generate_layer2_report(void)
{
    kprintf("📊 Layer 2 Status Report\n");
    kprintf("========================\n");

    /* Memory statistics */
    kprintf("Memory Management:\n");
    kprintf("  Heap: %llu/%llu blocks used\n",
            kernel_heap.used_blocks, kernel_heap.total_blocks);
    kprintf("  Total allocations: %u\n", allocations_total);
    kprintf("  Total frees: %u\n", frees_total);
    kprintf("  Memory leaks: %d blocks\n",
            (int)allocations_total - (int)frees_total);

    /* Thread statistics */
    kprintf("Thread Management:\n");
    kprintf("  Thread switches: %u\n", thread_switches);
    kprintf("  Switch rate: ~%u/sec\n", thread_switches / 10);  /* Assuming 10 seconds */

    /* Syscall statistics */
    kprintf("System Calls:\n");
    kprintf("  Total syscalls: %u\n", syscalls_executed);

    /* Timer statistics */
    uint64_t current_ticks = timer_get_ticks();
    kprintf("Timer:\n");
    kprintf("  Current ticks: %llu\n", current_ticks);
    kprintf("  Uptime: ~%llu seconds\n", current_ticks / 100);

    /* Overall health */
    int health_score = 100;
    if (allocations_total != frees_total) health_score -= 20;
    if (thread_switches < 100) health_score -= 10;
    if (syscalls_executed < 50) health_score -= 10;

    kprintf("Overall Health: %d%%\n", health_score);

    if (health_score >= 90) {
        kprintf("✅ Layer 2: EXCELLENT\n");
    } else if (health_score >= 70) {
        kprintf("⚠️  Layer 2: GOOD (minor issues)\n");
    } else {
        kprintf("❌ Layer 2: NEEDS ATTENTION\n");
    }
}