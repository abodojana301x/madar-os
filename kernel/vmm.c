/*
 * Layer: 2 (Kernel Core) - Virtual Memory Manager Implementation
 * File: vmm.c
 * Purpose: Basic identity mapping for kernel
 * Status: Simplified for Layer 2
 */

#include <types.h>
#include <vmm.h>
#include <memory.h>
#include <memory_layout.h>
#include <kprintf.h>
#include <string.h>

/* Kernel Page Tables */
pml4e_t *kernel_pml4;

/*
 * Initialize Virtual Memory Manager
 */
void vmm_init(void)
{
    kprintf("Initializing Virtual Memory Manager...\n");

    /* Allocate kernel PML4 */
    kernel_pml4 = (pml4e_t *)pmm_alloc_page();
    if (!kernel_pml4) {
        kprintf("Failed to allocate kernel PML4\n");
        return;
    }

    /* Clear PML4 */
    memset(kernel_pml4, 0, PAGE_SIZE);

    /* For now, just use identity mapping */
    /* We'll implement full 4-level paging later */

    kprintf("VMM initialized with basic identity mapping\n");
}

/*
 * Map a page (stub implementation)
 */
void vmm_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags)
{
    /* Stub - for now assume identity mapping */
    (void)virtual_addr;
    (void)physical_addr;
    (void)flags;
}

/*
 * Unmap a page (stub implementation)
 */
void vmm_unmap_page(uint64_t virtual_addr)
{
    /* Stub */
    (void)virtual_addr;
}

/*
 * Get physical address (stub implementation)
 */
uint64_t vmm_get_physical_address(uint64_t virtual_addr)
{
    /* For identity mapping */
    return virtual_addr;
}

/*
 * Flush TLB
 */
void vmm_flush_tlb(uint64_t virtual_addr)
{
    __asm__ volatile("invlpg (%0)" : : "r"(virtual_addr));
}/*
 * Layer: 2 (Kernel Core) - Virtual Memory Manager Implementation
 * File: vmm.c
 * Purpose: Implement 4-level paging with identity mapping
 * Status: New for Layer 2
 */

#include <vmm.h>
#include <memory.h>
#include <kprintf.h>

/* Kernel Page Tables */
pml4e_t *kernel_pml4;

/*
 * Initialize Virtual Memory Manager
 */
void vmm_init(void)
{
    kprintf("Initializing Virtual Memory Manager...\n");

    /* Allocate kernel PML4 */
    kernel_pml4 = (pml4e_t *)pmm_alloc_page();
    if (!kernel_pml4) {
        kprintf("Failed to allocate kernel PML4\n");
        return;
    }

    /* Clear PML4 */
    memset(kernel_pml4, 0, PAGE_SIZE);

    /* Identity map first 2GB of physical memory using 2MB pages */
    kprintf("  Identity mapping 0x00000000 - 0x80000000...\n");
    for (uint64_t addr = 0; addr < PHYSICAL_MEMORY_END; addr += PAGE_SIZE_2MB) {
        vmm_map_page(addr, addr, PTE_PRESENT | PTE_WRITABLE);
    }

    /* Load CR3 with kernel PML4 */
    __asm__ volatile("mov %0, %%cr3" : : "r"(kernel_pml4));

    kprintf("VMM initialized with identity mapping\n");
}

/*
 * Map a page (4KB or 2MB)
 */
void vmm_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags)
{
    /* For simplicity, use 2MB pages for identity mapping */
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdp_index = PDP_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);

    /* Get or create PDP */
    pdpe_t *pdp;
    if (!(kernel_pml4[pml4_index] & PTE_PRESENT)) {
        pdp = (pdpe_t *)pmm_alloc_page();
        if (!pdp) return;
        memset(pdp, 0, PAGE_SIZE);
        kernel_pml4[pml4_index] = (uint64_t)pdp | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pdp = (pdpe_t *)(kernel_pml4[pml4_index] & ~0xFFFULL);
    }

    /* Get or create PD */
    pde_t *pd;
    if (!(pdp[pdp_index] & PTE_PRESENT)) {
        pd = (pde_t *)pmm_alloc_page();
        if (!pd) return;
        memset(pd, 0, PAGE_SIZE);
        pdp[pdp_index] = (uint64_t)pd | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pd = (pde_t *)(pdp[pdp_index] & ~0xFFFULL);
    }

    /* Map 2MB page */
    pd[pd_index] = (physical_addr & ~0x1FFFFFULL) | flags | PTE_PAGE_SIZE;
}

/*
 * Unmap a page
 */
void vmm_unmap_page(uint64_t virtual_addr)
{
    /* Simplified - just invalidate */
    vmm_flush_tlb(virtual_addr);
}

/*
 * Get physical address for virtual address
 */
uint64_t vmm_get_physical_address(uint64_t virtual_addr)
{
    /* For identity mapping, virtual == physical */
    return virtual_addr;
}

/*
 * Flush TLB entry
 */
void vmm_flush_tlb(uint64_t virtual_addr)
{
    __asm__ volatile("invlpg (%0)" : : "r"(virtual_addr));
}"🧠 Initializing Virtual Memory Manager...\n");
 - Virtual Memory Manager Implementation
 * File: vmm.c
 * Purpose: Implement 4-level paging with identity mapping
 * Status: New for Layer 2
 */

#include <vmm.h>
#include <memory.h>
#include <kprintf.h>

/* Kernel Page Tables */
pml4e_t *kernel_pml4;

/* Static functions */
static pml4e_t *create_page_table(void);
static void map_2mb_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);

/*
 * Initialize Virtual Memory Manager
 */
void vmm_init(void)
{
    kprintf("🧠 Initializing Virtual Memory Manager...\n");

    /* Allocate kernel PML4 */
    kernel_pml4 = (pml4e_t *)pmm_alloc_page();
    if (!kernel_pml4) {
        kprintf("❌ Failed to allocate kernel PML4\n");
        return;
    }

    /* Clear PML4 */
    memset(kernel_pml4, 0, PAGE_SIZE);

    /* Identity map first 2GB of physical memory */
    kprintf("  Identity mapping 0x00000000 - 0x80000000...\n");
    for (uint64_t addr = 0; addr < PHYSICAL_MEMORY_END; addr += PAGE_SIZE_2MB) {
        map_2mb_page(addr, addr, PTE_PRESENT | PTE_WRITABLE);
    }

    /* Load CR3 with kernel PML4 */
    __asm__ volatile("mov %0, %%cr3" : : "r"(kernel_pml4));

    kprintf("✅ VMM initialized with identity mapping\n");
}

/*
 * Map a 4KB page
 */
void vmm_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags)
{
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdp_index = PDP_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);

    /* Get PDP */
    pdpe_t *pdp;
    if (!(kernel_pml4[pml4_index] & PTE_PRESENT)) {
        pdp = (pdpe_t *)pmm_alloc_page();
        if (!pdp) return;
        memset(pdp, 0, PAGE_SIZE);
        kernel_pml4[pml4_index] = (uint64_t)pdp | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pdp = (pdpe_t *)(kernel_pml4[pml4_index] & ~0xFFFULL);
    }

    /* Get PD */
    pde_t *pd;
    if (!(pdp[pdp_index] & PTE_PRESENT)) {
        pd = (pde_t *)pmm_alloc_page();
        if (!pd) return;
        memset(pd, 0, PAGE_SIZE);
        pdp[pdp_index] = (uint64_t)pd | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pd = (pde_t *)(pdp[pdp_index] & ~0xFFFULL);
    }

    /* Get PT */
    pte_t *pt;
    if (!(pd[pd_index] & PTE_PRESENT)) {
        pt = (pte_t *)pmm_alloc_page();
        if (!pt) return;
        memset(pt, 0, PAGE_SIZE);
        pd[pd_index] = (uint64_t)pt | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pt = (pte_t *)(pd[pd_index] & ~0xFFFULL);
    }

    /* Map the page */
    pt[pt_index] = (physical_addr & ~0xFFFULL) | flags;
}

/*
 * Unmap a page
 */
void vmm_unmap_page(uint64_t virtual_addr)
{
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdp_index = PDP_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);

    /* Navigate to PT */
    if (!(kernel_pml4[pml4_index] & PTE_PRESENT)) return;
    pdpe_t *pdp = (pdpe_t *)(kernel_pml4[pml4_index] & ~0xFFFULL);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return;
    pde_t *pd = (pde_t *)(pdp[pdp_index] & ~0xFFFULL);

    if (!(pd[pd_index] & PTE_PRESENT)) return;
    pte_t *pt = (pte_t *)(pd[pd_index] & ~0xFFFULL);

    /* Unmap the page */
    pt[pt_index] = 0;

    /* Flush TLB */
    vmm_flush_tlb(virtual_addr);
}

/*
 * Get physical address for virtual address
 */
uint64_t vmm_get_physical_address(uint64_t virtual_addr)
{
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdp_index = PDP_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);

    /* Navigate to PTE */
    if (!(kernel_pml4[pml4_index] & PTE_PRESENT)) return 0;
    pdpe_t *pdp = (pdpe_t *)(kernel_pml4[pml4_index] & ~0xFFFULL);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return 0;
    pde_t *pd = (pde_t *)(pdp[pdp_index] & ~0xFFFULL);

    if (!(pd[pd_index] & PTE_PRESENT)) return 0;
    pte_t *pt = (pte_t *)(pd[pd_index] & ~0xFFFULL);

    if (!(pt[pt_index] & PTE_PRESENT)) return 0;

    return (pt[pt_index] & ~0xFFFULL) + (virtual_addr & 0xFFFULL);
}

/*
 * Flush TLB entry
 */
void vmm_flush_tlb(uint64_t virtual_addr)
{
    __asm__ volatile("invlpg (%0)" : : "r"(virtual_addr));
}

/*
 * Map a 2MB page (for identity mapping)
 */
static void map_2mb_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags)
{
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdp_index = PDP_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);

    /* Get PDP */
    pdpe_t *pdp;
    if (!(kernel_pml4[pml4_index] & PTE_PRESENT)) {
        pdp = (pdpe_t *)pmm_alloc_page();
        if (!pdp) return;
        memset(pdp, 0, PAGE_SIZE);
        kernel_pml4[pml4_index] = (uint64_t)pdp | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pdp = (pdpe_t *)(kernel_pml4[pml4_index] & ~0xFFFULL);
    }

    /* Get PD */
    pde_t *pd;
    if (!(pdp[pdp_index] & PTE_PRESENT)) {
        pd = (pde_t *)pmm_alloc_page();
        if (!pd) return;
        memset(pd, 0, PAGE_SIZE);
        pdp[pdp_index] = (uint64_t)pd | PTE_PRESENT | PTE_WRITABLE;
    } else {
        pd = (pde_t *)(pdp[pdp_index] & ~0xFFFULL);
    }

    /* Map 2MB page */
    pd[pd_index] = (physical_addr & ~0x1FFFFFULL) | flags | PTE_PAGE_SIZE