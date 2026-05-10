/*
 * Layer: 1 (HAL/Bootstrap)
 * Purpose: تحديد خريطة الذاكرة للنواة
 * Constraints: لا يتجاوز 50KB في هذه المرحلة
 */

ENTRY(boot_start)
DRIVER_BASE = 0x500000;

PHDRS
{
    text PT_LOAD FLAGS(5); /* read + execute */
    data PT_LOAD FLAGS(6); /* read + write */
}

SECTIONS
{
    . = 1M;

    .multiboot :
    {
        KEEP(*(.multiboot))
    } :text

    .boot :
    {
        __boot_start = .;
        *(.boot)
        __boot_end = .;
    } :text

    /* Symbols for memory layout */
    __kernel_start = .;
    __physical_load_address = .;

    .text :
    {
        __text_start = .;
        *(.text)
        __text_end = .;
    } :text

    .rodata :
    {
        __rodata_start = .;
        *(.rodata)
        __rodata_end = .;
    } :text

    .data :
    {
        __data_start = .;
        *(.data)
        __data_end = .;
    } :data

    .bss :
    {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    } :data

    __kernel_end = .;

    /* Driver reserves (physical addresses) */
    __driver_start = DRIVER_BASE;
    . = ASSERT(. <= DRIVER_BASE + 0x100000, "Drivers overflow");

}
