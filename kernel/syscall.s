/*
 * Layer: 2 (Kernel Core) - Syscall Entry Point
 * File: syscall.s
 * Purpose: Assembly syscall handler
 * Status: New for Layer 2
 */

.global syscall_handler

/*
 * Syscall handler
 */
syscall_handler:
    /* Save user registers */
    push %rcx    /* Return RIP */
    push %r11    /* Return RFLAGS */

    /* Save all registers */
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    /* Call C dispatcher */
    mov %rax, %rdi          /* syscall number */
    call syscall_dispatcher

    /* Restore registers */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax

    /* Return to user */
    pop %r11    /* Restore RFLAGS */
    pop %rcx    /* Restore RIP */
    sysret