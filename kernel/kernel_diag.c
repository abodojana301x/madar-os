#include <kernel_diag.h>
#include <heap.h>
#include <kprintf.h>
#include <process.h>
#include <scheduler.h>
#include <sync.h>
#include <syscall.h>
#include <timer.h>

void kernel_diag_print_snapshot(void)
{
    heap_stats_t heap_stats;
    process_stats_t process_stats;
    scheduler_stats_t scheduler_stats;
    syscall_stats_t syscall_stats;
    mutex_stats_t mutex_stats;

    heap_get_stats(&heap_stats);
    process_get_stats(&process_stats);
    scheduler_get_stats(&scheduler_stats);
    syscall_get_stats(&syscall_stats);
    mutex_get_stats(&mutex_stats);

    kprintf("Kernel diagnostics snapshot:\n");
    kprintf("  heap: used=%llu peak=%llu total=%llu alloc=%llu free=%llu fail=%llu bad_free=%llu\n",
            heap_stats.used_blocks,
            heap_stats.peak_used_blocks,
            heap_stats.total_blocks,
            heap_stats.alloc_count,
            heap_stats.free_count,
            heap_stats.failed_allocations,
            heap_stats.invalid_frees);
    kprintf("  proc: live=%u ready=%u run=%u block=%u exit=%u created=%u destroyed=%u\n",
            process_stats.live_threads,
            process_stats.ready_threads,
            process_stats.running_threads,
            process_stats.blocked_threads,
            process_stats.exited_threads,
            process_stats.total_created,
            process_stats.total_destroyed);
    kprintf("  sched: ticks=%llu schedules=%llu switches=%llu yields=%llu quantum=%llu idle=%llu\n",
            scheduler_stats.tick_count,
            scheduler_stats.schedule_count,
            scheduler_stats.context_switch_count,
            scheduler_stats.yields,
            scheduler_stats.quantum_expirations,
            scheduler_stats.idle_returns);
    kprintf("  timer: ticks=%llu\n", timer_get_ticks());
    kprintf("  syscalls: total=%llu invalid=%llu stubs=%llu\n",
            syscall_stats.total_calls,
            syscall_stats.invalid_calls,
            syscall_stats.stub_calls);
    kprintf("  sync: lock=%llu try=%llu unlock=%llu contention=%llu owner_violation=%llu\n",
            mutex_stats.lock_calls,
            mutex_stats.try_lock_calls,
            mutex_stats.unlock_calls,
            mutex_stats.contention_loops,
            mutex_stats.owner_violations);
}
