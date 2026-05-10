#include <panic.h>
#include <kprintf.h>
#include <vga.h>
#include <serial.h>

typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, cs, rflags;
} registers_t;

static void crash_draw_banner(void)
{
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_clear();
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_RED);
    vga_puts("=== MADAR OS KERNEL PANIC ===\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
}

void crash_handler_dump(registers_t *regs, const char *reason)
{
    crash_draw_banner();

    kprintf("\n*** KERNEL PANIC ***\n");
    kprintf("Reason: %s\n\n", reason ? reason : "Unknown");

    if (regs) {
        kprintf("Registers:\n");
        kprintf("  RAX=0x%016llx  RBX=0x%016llx\n", regs->rax, regs->rbx);
        kprintf("  RCX=0x%016llx  RDX=0x%016llx\n", regs->rcx, regs->rdx);
        kprintf("  RSI=0x%016llx  RDI=0x%016llx\n", regs->rsi, regs->rdi);
        kprintf("  RBP=0x%016llx  RSP=0x%016llx\n", regs->rbp, regs->rsp);
        kprintf("  R8 =0x%016llx  R9 =0x%016llx\n", regs->r8, regs->r9);
        kprintf("  R10=0x%016llx  R11=0x%016llx\n", regs->r10, regs->r11);
        kprintf("  R12=0x%016llx  R13=0x%016llx\n", regs->r12, regs->r13);
        kprintf("  R14=0x%016llx  R15=0x%016llx\n", regs->r14, regs->r15);
        kprintf("  RIP=0x%016llx  CS=0x%04llx\n", regs->rip, regs->cs);
        kprintf("  RFLAGS=0x%08llx\n", regs->rflags);
    }

    serial_puts("Kernel panic - system halted.\n");

    asm volatile("cli");
    while (1) { asm volatile("hlt"); }
}

void crash_handler(const char *reason)
{
    registers_t regs;
    asm volatile(
        "mov %%rax, %0\n"
        "mov %%rbx, %1\n"
        "mov %%rcx, %2\n"
        "mov %%rdx, %3\n"
        "mov %%rsi, %4\n"
        "mov %%rdi, %5\n"
        "mov %%rbp, %6\n"
        "mov %%rsp, %7\n"
        "mov %%r8,  %8\n"
        "mov %%r9,  %9\n"
        "mov %%r10, %10\n"
        "mov %%r11, %11\n"
        "mov %%r12, %12\n"
        "mov %%r13, %13\n"
        "mov %%r14, %14\n"
        "mov %%r15, %15\n"
        : "=m"(regs.rax), "=m"(regs.rbx), "=m"(regs.rcx), "=m"(regs.rdx),
          "=m"(regs.rsi), "=m"(regs.rdi), "=m"(regs.rbp), "=m"(regs.rsp),
          "=m"(regs.r8), "=m"(regs.r9), "=m"(regs.r10), "=m"(regs.r11),
          "=m"(regs.r12), "=m"(regs.r13), "=m"(regs.r14), "=m"(regs.r15)
        :
        : "memory"
    );
    regs.rip = 0;
    regs.cs = 0;
    regs.rflags = 0;

    crash_handler_dump(&regs, reason);
}
