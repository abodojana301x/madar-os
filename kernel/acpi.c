/*
 * Layer: 2 (Kernel Core) - ACPI Implementation
 * File: acpi.c
 * Purpose: Parse ACPI tables for system configuration
 */

#include <acpi.h>
#include <kprintf.h>
#include <string.h>

/* Global ACPI state */
static acpi_rsdp_t g_rsdp;
static acpi_fadt_t *g_fadt = NULL;
static acpi_madt_t *g_madt = NULL;
static acpi_info_t g_acpi_info = {
    .valid = 0,
    .revision = 0,
    .cpu_count = 0,
    .lapic_addr = 0,
    .has_8259 = 0,
    .sci_interrupt = 0
};

/* Verify ACPI table checksum */
static int acpi_checksum_valid(void *table, uint32_t length) {
    uint8_t *bytes = (uint8_t *)table;
    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += bytes[i];
    }
    return sum == 0;
}

/* Search for RSDP in BIOS memory */
static int acpi_find_rsdp_internal(void) {
    kprintf("🔍 Searching for ACPI RSDP...\n");
    
    for (uint32_t addr = ACPI_RSDP_SEARCH_START; addr <= ACPI_RSDP_SEARCH_END; addr += ACPI_RSDP_SEARCH_STEP) {
        acpi_rsdp_t *rsdp = (acpi_rsdp_t *)(uintptr_t)addr;
        
        /* Check signature */
        if (strncmp((const char *)rsdp->signature, ACPI_SIG_RSDP, 8) != 0) {
            continue;
        }
        
        /* Verify checksum of first 20 bytes (ACPI 1.0 checksum) */
        uint8_t checksum = 0;
        uint8_t *bytes = (uint8_t *)rsdp;
        for (int i = 0; i < 20; i++) {
            checksum += bytes[i];
        }
        
        if (checksum != 0) {
            continue;
        }
        
        /* For ACPI 2.0+, verify extended checksum */
        if (rsdp->revision >= 2) {
            checksum = 0;
            for (uint32_t i = 0; i < rsdp->length; i++) {
                checksum += bytes[i];
            }
            if (checksum != 0) {
                continue;
            }
        }
        
        /* Found valid RSDP */
        memcpy(&g_rsdp, rsdp, sizeof(acpi_rsdp_t));
        
        kprintf("✅ RSDP found at 0x%X\n", addr);
        kprintf("   OEM: %.6s\n", (char *)g_rsdp.oem_id);
        kprintf("   Revision: %u (ACPI %u.0)\n", g_rsdp.revision, g_rsdp.revision >= 2 ? 2 : 1);
        kprintf("   RSDT: 0x%X\n", g_rsdp.rsdt_addr);
        if (g_rsdp.revision >= 2) {
            kprintf("   XSDT: 0x%llX\n", (uint64_t)g_rsdp.xsdt_addr);
        }
        
        return 0;
    }
    
    kprintf("⚠️  ACPI RSDP not found in BIOS memory\n");
    return -1;
}

/* Find ACPI table by signature */
static uint32_t acpi_find_table_addr(const char *signature) {
    if (g_rsdp.rsdt_addr == 0) {
        return 0;
    }
    
    acpi_rsdt_t *rsdt = (acpi_rsdt_t *)(uintptr_t)g_rsdp.rsdt_addr;
    
    /* Verify RSDT header */
    if (strncmp((const char *)rsdt->header.signature, "RSDT", 4) != 0) {
        return 0;
    }
    
    if (!acpi_checksum_valid(rsdt, rsdt->header.length)) {
        return 0;
    }
    
    /* Calculate number of entries */
    uint32_t entry_count = (rsdt->header.length - sizeof(acpi_header_t)) / sizeof(uint32_t);
    uint32_t *entries = (uint32_t *)((uint8_t *)rsdt + sizeof(acpi_header_t));
    
    for (uint32_t i = 0; i < entry_count; i++) {
        acpi_header_t *table = (acpi_header_t *)(uintptr_t)entries[i];
        if (strncmp((const char *)table->signature, signature, 4) == 0) {
            return entries[i];
        }
    }
    
    return 0;
}

/* Parse FADT (Fixed ACPI Description Table) */
static int acpi_parse_fadt_internal(void) {
    uint32_t fadt_addr = acpi_find_table_addr(ACPI_SIG_FADT);
    if (fadt_addr == 0) {
        kprintf("⚠️  FADT table not found\n");
        return -1;
    }
    
    g_fadt = (acpi_fadt_t *)(uintptr_t)fadt_addr;
    
    if (!acpi_checksum_valid(g_fadt, g_fadt->header.length)) {
        kprintf("⚠️  FADT checksum invalid\n");
        return -1;
    }
    
    kprintf("✅ FADT found at 0x%X\n", fadt_addr);
    kprintf("   SCI Interrupt: %u\n", g_fadt->sci_interrupt);
    kprintf("   Preferred PM Profile: %u\n", g_fadt->preferred_pm_profile);
    
    g_acpi_info.sci_interrupt = g_fadt->sci_interrupt;
    
    return 0;
}

/* Parse MADT (Multiple APIC Description Table) */
static int acpi_parse_madt_internal(void) {
    uint32_t madt_addr = acpi_find_table_addr(ACPI_SIG_MADT);
    if (madt_addr == 0) {
        kprintf("⚠️  MADT table not found\n");
        return -1;
    }
    
    g_madt = (acpi_madt_t *)(uintptr_t)madt_addr;
    
    if (!acpi_checksum_valid(g_madt, g_madt->header.length)) {
        kprintf("⚠️  MADT checksum invalid\n");
        return -1;
    }
    
    kprintf("✅ MADT found at 0x%X\n", madt_addr);
    kprintf("   Local APIC Address: 0x%X\n", g_madt->lapic_addr);
    kprintf("   Flags: 0x%X (8259 %s)\n", g_madt->flags, 
            (g_madt->flags & 1) ? "present" : "absent");
    
    g_acpi_info.lapic_addr = g_madt->lapic_addr;
    g_acpi_info.has_8259 = (g_madt->flags & 1) ? 1 : 0;
    
    /* Parse MADT controller structures for CPU count */
    uint8_t *ics_ptr = (uint8_t *)g_madt + sizeof(acpi_madt_t);
    uint8_t *madt_end = (uint8_t *)g_madt + g_madt->header.length;
    uint32_t cpu_count = 0;
    
    while (ics_ptr < madt_end) {
        uint8_t type = ics_ptr[0];
        uint8_t length = ics_ptr[1];
        
        if (length == 0) break;
        
        if (type == 0) {  /* Processor Local APIC */
            acpi_madt_lapic_t *lapic = (acpi_madt_lapic_t *)ics_ptr;
            if (lapic->flags & 1) {  /* Enabled */
                cpu_count++;
                kprintf("   CPU %u: APIC ID %u\n", cpu_count - 1, lapic->apic_id);
            }
        } else if (type == 1) {  /* I/O APIC */
            acpi_madt_ioapic_t *ioapic = (acpi_madt_ioapic_t *)ics_ptr;
            kprintf("   I/O APIC ID %u: Address 0x%X, GSI Base %u\n",
                    ioapic->ioapic_id, ioapic->ioapic_addr, ioapic->gsi_base);
        } else if (type == 2) {  /* Interrupt Source Override */
            acpi_madt_iso_t *iso = (acpi_madt_iso_t *)ics_ptr;
            kprintf("   IRQ Override: Source %u -> GSI %u\n", iso->source, iso->gsi);
        }
        
        ics_ptr += length;
    }
    
    g_acpi_info.cpu_count = cpu_count;
    kprintf("   Total CPUs: %u\n", cpu_count);
    
    return 0;
}

/* Public API: Initialize ACPI */
void acpi_init(void) {
    kprintf("🔧 ACPI Initialization\n");
    kprintf("──────────────────────────────\n");
    
    memset(&g_rsdp, 0, sizeof(g_rsdp));
    memset(&g_acpi_info, 0, sizeof(g_acpi_info));
    
    /* Find RSDP */
    if (acpi_find_rsdp_internal() != 0) {
        kprintf("⚠️  ACPI disabled (RSDP not found - using defaults)\n");
        return;
    }
    
    /* Parse tables */
    if (acpi_parse_tables() != 0) {
        kprintf("⚠️  ACPI parsing failed - using defaults\n");
        return;
    }
    
    g_acpi_info.valid = 1;
    g_acpi_info.revision = g_rsdp.revision;
    
    kprintf("✅ ACPI ready\n");
    kprintf("──────────────────────────────\n");
}

/* Public API: Find RSDP */
int acpi_find_rsdp(void) {
    return acpi_find_rsdp_internal();
}

/* Public API: Parse all ACPI tables */
int acpi_parse_tables(void) {
    int result = 0;
    
    if (acpi_parse_fadt_internal() != 0) {
        result = -1;
    }
    
    if (acpi_parse_madt_internal() != 0) {
        result = -1;
    }
    
    return result;
}

/* Public API: Parse FADT */
int acpi_parse_fadt(void) {
    return acpi_parse_fadt_internal();
}

/* Public API: Parse MADT */
int acpi_parse_madt(void) {
    return acpi_parse_madt_internal();
}

/* Public API: Get ACPI info */
void acpi_get_info(acpi_info_t *info) {
    if (info) {
        memcpy(info, &g_acpi_info, sizeof(acpi_info_t));
    }
}

/* Public API: Print ACPI info */
void acpi_print_info(void) {
    kprintf("\n📋 ACPI System Information:\n");
    kprintf("   Valid: %s\n", g_acpi_info.valid ? "YES" : "NO");
    kprintf("   Revision: %u (ACPI %u.0)\n", g_acpi_info.revision,
            g_acpi_info.revision >= 2 ? 2 : 1);
    kprintf("   CPU Count: %u\n", g_acpi_info.cpu_count);
    kprintf("   Local APIC Address: 0x%X\n", g_acpi_info.lapic_addr);
    kprintf("   8259 PIC Present: %s\n", g_acpi_info.has_8259 ? "YES" : "NO");
    kprintf("   SCI Interrupt: %d\n", g_acpi_info.sci_interrupt);
}

/* Debug: Dump RSDP info */
void acpi_dump_rsdp(void) {
    if (g_rsdp.rsdt_addr == 0) {
        kprintf("RSDP: Not found\n");
        return;
    }
    
    kprintf("RSDP Dump:\n");
    kprintf("  Signature: %.8s\n", (char *)g_rsdp.signature);
    kprintf("  OEM: %.6s\n", (char *)g_rsdp.oem_id);
    kprintf("  Revision: %u\n", g_rsdp.revision);
    kprintf("  RSDT Address: 0x%X\n", g_rsdp.rsdt_addr);
    kprintf("  XSDT Address: 0x%llX\n", (uint64_t)g_rsdp.xsdt_addr);
}
