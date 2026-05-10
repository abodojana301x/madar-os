/*
 * Layer: 2 Context Switch Assembly
 * Save/restore all registers for thread switch
 */

.intel_syntax noprefix
.global context_switch

context_switch:
    /* Save current context to PCB */
    mov rax, rdi        /* rdi = current_pcb */
    
    /* Save all callee-saved + general registers */
    push r15
    push r14
    push r13
    push r12
    push rbx
    push rbp
    
    push r11
    push r10
    push r9
    push r8
    
    push rax  /* Save original RAX */
    push rcx
    push rdx
    push rsi
    push rdi
    
    mov [rax + PCB_R15], r15
    mov [rax + PCB_R14], r14
    mov [rax + PCB_R13], r13
    mov [rax + PCB_R12], r12
    mov [rax + PCB_RBX], rbx
    mov [rax + PCB_RBP], rbp
    mov [rax + PCB_R11], r11
    mov [rax + PCB_R10], r10
    mov [rax + PCB_R9], r9
    mov [rax + PCB_R8], r8
    mov [rax + PCB_RAX], rax  /* Overwritten */
    mov [rax + PCB_RCX], rcx
    mov [rax + PCB_RDX], rdx
    mov [rax + PCB_RSI], rsi
    mov [rax + PCB_RDI], rdi
    
    pushfq
    pop qword [rax + PCB_RFLAGS]
    
    mov [rax + PCB_RSP], rsp  /* Update RSP */
    mov r10, [rsp]
    mov [rax + PCB_RIP], r10  /* RIP from stack */
    
    /* Load next context (next_pcb in rsi) */
    mov rsp, [rsi + PCB_RSP]
    mov r15, [rsi + PCB_R15]
    mov r14, [rsi + PCB_R14]
    mov r13, [rsi + PCB_R13]
    mov r12, [rsi + PCB_R12]
    mov rbx, [rsi + PCB_RBX]
    mov rbp, [rsi + PCB_RBP]
    mov r11, [rsi + PCB_R11]
    mov r10, [rsi + PCB_R10]
    mov r9, [rsi + PCB_R9]
    mov r8, [rsi + PCB_R8]
    mov rax, [rsi + PCB_RAX]
    mov rcx, [rsi + PCB_RCX]
    mov rdx, [rsi + PCB_RDX]
    mov rdi, [rsi + PCB_RDI]
    
    popfq
    retq  /* Return to next RIP via iretq in scheduler */
