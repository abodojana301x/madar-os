/*
 * Layer: 1 (HAL/Bootstrap)
 * File: boot.s
 * Purpose: Multiboot2 entry + transition to long mode
 */

.intel_syntax noprefix

.set MB2_HEADER_MAGIC, 0xE85250D6
.set MB2_BOOT_MAGIC,   0x36D76289
.set MB2_ARCH_I386,    0
.set MB2_HEADER_LEN,   48
.set MB2_CHECKSUM, -(MB2_HEADER_MAGIC + MB2_ARCH_I386 + MB2_HEADER_LEN)

.set CR0_PE_PG, 0x80000001
.set CR4_PAE,   0x20
.set EFER_MSR,  0xC0000080
.set EFER_LME,  0x100

.set STACK_SIZE, 0x8000

/* Multiboot2 header: must be within first 32 KiB, 8-byte aligned */
.section .multiboot, "a"
.align 8
multiboot_header:
    .long MB2_HEADER_MAGIC
    .long MB2_ARCH_I386
    .long MB2_HEADER_LEN
    .long MB2_CHECKSUM

    /* Framebuffer header tag (request 1024x768x32) */
    .short 5
    .short 0
    .long 20
    .long 1024
    .long 768
    .long 32
    .long 0

    /* End tag */
    .short 0
    .short 0
    .long 8

.section .boot, "ax"
.code32
.align 16
.global boot_start
.extern kernel_main

boot_start:
    cli

    cmp eax, MB2_BOOT_MAGIC
    jne boot_error_magic

    mov dword ptr [mb_magic], eax
    mov dword ptr [mb_info], ebx

    mov esp, offset stack_top
    and esp, 0xFFFFFFF0

    call setup_page_tables
    test eax, eax
    jnz boot_error_paging

    call enable_long_mode
    test eax, eax
    jnz boot_error_longmode

    lgdt [gdt64_descriptor]
    jmp 0x08:long_mode_entry

boot_error_magic:
    mov al, 0x01
    mov byte ptr [boot_error_code], al
    jmp boot_error_screen

boot_error_paging:
    mov al, 0x02
    mov byte ptr [boot_error_code], al
    jmp boot_error_screen

boot_error_longmode:
    mov al, 0x03
    mov byte ptr [boot_error_code], al
    jmp boot_error_screen

boot_hang:
    cli
    hlt
    jmp boot_hang

boot_error_screen:
    /* Write error code to VGA text buffer at 0xB8000 */
    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x1F00 | ' '
    rep stosw

    mov edi, 0xB8000
    mov esi, offset boot_error_msg_prefix
    call boot_write_string

    movzx eax, byte ptr [boot_error_code]
    add al, '0'
    mov ah, 0x1F
    mov [edi], ax

    hlt
    jmp boot_error_screen

boot_write_string:
    lodsb
    test al, al
    jz boot_write_done
    mov ah, 0x1F
    stosw
    jmp boot_write_string
boot_write_done:
    ret

setup_page_tables:
    /* Clear PML4, PDPT, and PD tables (6 pages total) */
    xor eax, eax
    mov edi, offset pml4_table
    mov ecx, (4096 * 6) / 4
    rep stosd

    /* PML4[0] -> PDPT */
    mov eax, offset pdpt_table
    or eax, 0x03
    mov dword ptr [pml4_table], eax
    mov dword ptr [pml4_table + 4], 0

    /* Validate PML4 entry was written */
    mov eax, dword ptr [pml4_table]
    test eax, 0x03
    jz .paging_fail

    /* PDPT[0..3] -> PD0..PD3 */
    mov eax, offset pd_table0
    or eax, 0x03
    mov dword ptr [pdpt_table], eax
    mov dword ptr [pdpt_table + 4], 0

    mov eax, offset pd_table1
    or eax, 0x03
    mov dword ptr [pdpt_table + 8], eax
    mov dword ptr [pdpt_table + 12], 0

    mov eax, offset pd_table2
    or eax, 0x03
    mov dword ptr [pdpt_table + 16], eax
    mov dword ptr [pdpt_table + 20], 0

    mov eax, offset pd_table3
    or eax, 0x03
    mov dword ptr [pdpt_table + 24], eax
    mov dword ptr [pdpt_table + 28], 0

    /* Identity map first 4 GiB with 2 MiB pages */
    mov edi, offset pd_table0
    xor ecx, ecx
.map_loop:
    mov eax, ecx
    shl eax, 21
    or eax, 0x83
    mov dword ptr [edi + ecx * 8], eax
    mov dword ptr [edi + ecx * 8 + 4], 0
    inc ecx
    cmp ecx, 2048
    jne .map_loop

    xor eax, eax
    ret

.paging_fail:
    mov eax, 1
    ret

enable_long_mode:
    mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax

    /* Verify PAE was enabled */
    mov eax, cr4
    test eax, CR4_PAE
    jz .longmode_fail

    mov eax, offset pml4_table
    mov cr3, eax

    mov ecx, EFER_MSR
    rdmsr
    or eax, EFER_LME
    wrmsr

    /* Verify LME was set */
    mov ecx, EFER_MSR
    rdmsr
    test eax, EFER_LME
    jz .longmode_fail

    mov eax, cr0
    or eax, CR0_PE_PG
    mov cr0, eax
    xor eax, eax
    ret

.longmode_fail:
    mov eax, 1
    ret

.code64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, offset stack_top
    and rsp, -16
    xor rbp, rbp

    mov edi, dword ptr [mb_magic]
    mov esi, dword ptr [mb_info]
    call kernel_main

.halt64:
    cli
    hlt
    jmp .halt64

.section .rodata
.align 16
boot_error_msg_prefix:
    .asciz "BOOT ERROR [Code: "
boot_error_code_prefix:
    .asciz "]\n 1=Bad Magic  2=Paging  3=Long Mode\nSystem Halted."
gdt64:
    .quad 0x0000000000000000
    .quad 0x00AF9A000000FFFF
    .quad 0x00AF92000000FFFF
gdt64_end:

gdt64_descriptor:
    .word gdt64_end - gdt64 - 1
    .long gdt64

.section .bss
.align 4096
pml4_table:
    .skip 4096
pdpt_table:
    .skip 4096
pd_table0:
    .skip 4096
pd_table1:
    .skip 4096
pd_table2:
    .skip 4096
pd_table3:
    .skip 4096

.align 16
mb_magic:
    .skip 4
mb_info:
    .skip 4

.align 16
boot_error_code:
    .skip 1
stack_bottom:
    .skip STACK_SIZE
stack_top:

.section .note.GNU-stack, "", @progbits
