/*
 * Layer: 2 (Kernel Core) - Syscall Implementation
 * File: syscall.c
 * Purpose: System call dispatcher and handlers
 * Status: New for Layer 2
 */

#include <syscall.h>
#include <kprintf.h>
#include <io.h>

static syscall_stats_t syscall_stats = {0};

/* Minimal syscall entry symbol used by MSR_LSTAR */
__attribute__((naked)) void syscall_handler(void)
{
    __asm__ volatile("ret");
}

/* Layer 4 syscall forward declarations */
extern uint64_t sys_open(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_read(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_write(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_close(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_mount(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_pipe(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_kill(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_setuid(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_chmod(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
static uint64_t sys_exec_app_stub(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
static uint64_t sys_http_request_stub(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
static uint64_t sys_list_procs_stub(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

/* Syscall table */
syscall_fn_t syscall_table[MAX_SYSCALLS] = {
    [SYS_PRINT] = sys_print,
    [SYS_EXIT] = sys_exit,
    [SYS_GET_TICK] = sys_get_tick,
    [SYS_OPEN] = sys_open,
    [SYS_READ] = sys_read,
    [SYS_WRITE] = sys_write,
    [SYS_CLOSE] = sys_close,
    [SYS_MOUNT] = sys_mount,
    [SYS_PIPE] = sys_pipe,
    [SYS_KILL] = sys_kill,
    [SYS_SETUID] = sys_setuid,
    [SYS_CHMOD] = sys_chmod,
    [SYS_EXEC_APP] = (syscall_fn_t)sys_exec_app_stub,
    [SYS_HTTP_REQUEST] = (syscall_fn_t)sys_http_request_stub,
    [SYS_LIST_PROCS] = (syscall_fn_t)sys_list_procs_stub,
};

/*
 * Initialize syscall interface
 */
void syscall_init(void)
{
    kprintf("🔧 Initializing Syscall Interface...\n");

    /* Set up MSR registers for syscall */
    uint64_t star_value = (0x0013000800000000ULL);  /* Kernel CS/SS | User CS/SS */
    uint64_t lstar_value = (uint64_t)syscall_handler;
    uint64_t sfmask_value = 0x200;  /* Mask interrupts */

    __asm__ volatile(
        "mov %0, %%rdx\n"
        "mov $0xc0000081, %%ecx\n"  /* IA32_STAR */
        "wrmsr\n"
        "mov %1, %%rdx\n"
        "mov $0xc0000082, %%ecx\n"  /* IA32_LSTAR */
        "wrmsr\n"
        "mov %2, %%rdx\n"
        "mov $0xc0000084, %%ecx\n"  /* IA32_SFMASK */
        "wrmsr\n"
        :
        : "r"(star_value), "r"(lstar_value), "r"(sfmask_value)
    );

    /* Enable syscall in EFER */
    uint64_t efer;
    __asm__ volatile(
        "mov $0xc0000080, %%ecx\n"  /* IA32_EFER */
        "rdmsr\n"
        "or $1, %%eax\n"           /* Set SCE bit */
        "wrmsr\n"
        : "=a"(efer)
    );

    kprintf("✅ Syscall interface initialized\n");
}

/*
 * Syscall handler (called from assembly)
 */
uint64_t syscall_dispatcher(uint64_t syscall_num,
                            uint64_t arg1, uint64_t arg2, uint64_t arg3,
                            uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
    syscall_stats.total_calls++;

    if (syscall_num >= MAX_SYSCALLS || !syscall_table[syscall_num]) {
        /* Invalid syscall */
        syscall_stats.invalid_calls++;
        return (uint64_t)-1;
    }

    /* Call the syscall handler */
    return syscall_table[syscall_num](arg1, arg2, arg3, arg4, arg5, arg6);
}

/*
 * System call implementations
 */
uint64_t sys_print(uint64_t str_ptr, uint64_t len, uint64_t, uint64_t, uint64_t, uint64_t)
{
    const char *str = (const char *)str_ptr;
    for (uint64_t i = 0; i < len && str[i]; i++) {
        /* For now, just use kprintf */
        kprintf("%c", str[i]);
    }
    return 0;
}

uint64_t sys_exit(uint64_t code, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)
{
    /* Exit current thread */
    extern void thread_exit(void);
    thread_exit();
    return code;
}

uint64_t sys_get_tick(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)
{
    extern uint64_t timer_get_ticks(void);
    return timer_get_ticks();
}

static uint64_t sys_exec_app_stub(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6)
{
    syscall_stats.stub_calls++;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;
    kprintf("sys_exec_app: not implemented yet\n");
    return (uint64_t)-1;
}

static uint64_t sys_http_request_stub(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6)
{
    syscall_stats.stub_calls++;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;
    kprintf("sys_http_request: not implemented yet\n");
    return (uint64_t)-1;
}

static uint64_t sys_list_procs_stub(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6)
{
    syscall_stats.stub_calls++;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;
    kprintf("sys_list_procs: not implemented yet\n");
    return (uint64_t)-1;
}

void syscall_get_stats(syscall_stats_t *stats)
{
    if (!stats) {
        return;
    }

    *stats = syscall_stats;
}
