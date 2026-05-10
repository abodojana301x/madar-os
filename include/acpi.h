/*
 * Layer: 2 (Kernel Core) - ACPI (Advanced Configuration and Power Interface)
 * File: acpi.h
 * Purpose: ACPI table parsing and system configuration from BIOS
 */

#ifndef _ACPI_H
#define _ACPI_H

#include "types.h"

/* ACPI Memory Search Range */
#define ACPI_RSDP_SEARCH_START  0xE0000
#define ACPI_RSDP_SEARCH_END    0xFFFFF
#define ACPI_RSDP_SEARCH_STEP   16  /* RSDP is 16-byte aligned */

/* ACPI Signatures */
#define ACPI_SIG_RSDP           "RSD PTR "
#define ACPI_SIG_RSDT           "RSDT"
#define ACPI_SIG_XSDT           "XSDT"
#define ACPI_SIG_FADT           "FACP"
#define ACPI_SIG_MADT           "APIC"
#define ACPI_SIG_DSDT           "DSDT"
#define ACPI_SIG_SSDT           "SSDT"

/* ACPI Table Header (common to most tables) */
typedef struct __attribute__((packed)) {
    uint8_t signature[4];       /* Table signature (e.g., "RSDT") */
    uint32_t length;            /* Length of table in bytes */
    uint8_t revision;           /* ACPI revision */
    uint8_t checksum;           /* Checksum (all fields sum to 0) */
    uint8_t oem_id[6];          /* OEM identification */
    uint8_t oem_table_id[8];    /* OEM table identification */
    uint32_t oem_revision;      /* OEM revision */
    uint32_t creator_id;        /* Creator ID */
    uint32_t creator_revision;  /* Creator revision */
} acpi_header_t;

/* RSDP (Root System Descriptor Pointer) - v2.0 */
typedef struct __attribute__((packed)) {
    uint8_t signature[8];       /* "RSD PTR " */
    uint8_t checksum;           /* Checksum of first 20 bytes */
    uint8_t oem_id[6];          /* OEM identification */
    uint8_t revision;           /* 0 = ACPI 1.0, 2+ = ACPI 2.0+ */
    uint32_t rsdt_addr;         /* Physical address of RSDT (32-bit) */
    /* Extended fields for ACPI 2.0+ */
    uint32_t length;            /* Length of RSDP (36 bytes for v2.0+) */
    uint64_t xsdt_addr;         /* Physical address of XSDT (64-bit) */
    uint8_t extended_checksum;  /* Checksum of entire RSDP */
    uint8_t reserved[3];        /* Reserved */
} acpi_rsdp_t;

/* RSDT (Root System Descriptor Table) - contains 32-bit table pointers */
typedef struct __attribute__((packed)) {
    acpi_header_t header;
    /* Followed by array of 32-bit table addresses */
} acpi_rsdt_t;

/* XSDT (Extended System Descriptor Table) - contains 64-bit table pointers */
typedef struct __attribute__((packed)) {
    acpi_header_t header;
    /* Followed by array of 64-bit table addresses */
} acpi_xsdt_t;

/* FADT (Fixed ACPI Description Table) - minimal version for basic info */
typedef struct __attribute__((packed)) {
    acpi_header_t header;
    uint32_t facs_addr;         /* Physical address of FACS */
    uint32_t dsdt_addr;         /* Physical address of DSDT */
    uint8_t  reserved;          /* Reserved */
    uint8_t  preferred_pm_profile;
    uint16_t sci_interrupt;     /* SCI interrupt number */
    uint32_t smi_cmd_port;      /* SMI command port */
    uint8_t  acpi_enable;       /* Value to enable ACPI */
    uint8_t  acpi_disable;      /* Value to disable ACPI */
    uint8_t  s4bios_request;    /* S4 BIOS request value */
    uint8_t  pstate_control;    /* PSTATE control value */
    uint32_t pm1a_event_block;  /* PM1a event block address */
    uint32_t pm1b_event_block;  /* PM1b event block address */
    /* Remaining fields truncated for Phase 2 - extend as needed */
} acpi_fadt_t;

/* MADT (Multiple APIC Description Table) - Interrupt Controller info */
typedef struct __attribute__((packed)) {
    acpi_header_t header;
    uint32_t lapic_addr;        /* Local APIC address */
    uint32_t flags;             /* Flags (bit 0 = 8259 present) */
    /* Followed by Interrupt Controller Structures (ICS) */
} acpi_madt_t;

/* MADT.ICS Type: Processor Local APIC */
typedef struct __attribute__((packed)) {
    uint8_t  type;              /* 0 = Processor Local APIC */
    uint8_t  length;            /* 8 bytes */
    uint8_t  acpi_processor_id;
    uint8_t  apic_id;
    uint32_t flags;             /* Bit 0 = enabled */
} acpi_madt_lapic_t;

/* MADT.ICS Type: I/O APIC */
typedef struct __attribute__((packed)) {
    uint8_t  type;              /* 1 = I/O APIC */
    uint8_t  length;            /* 12 bytes */
    uint8_t  ioapic_id;
    uint8_t  reserved;
    uint32_t ioapic_addr;       /* I/O APIC address */
    uint32_t gsi_base;          /* Global System Interrupt base */
} acpi_madt_ioapic_t;

/* MADT.ICS Type: Interrupt Source Override */
typedef struct __attribute__((packed)) {
    uint8_t  type;              /* 2 = Interrupt Source Override */
    uint8_t  length;            /* 10 bytes */
    uint8_t  bus;               /* Bus type (0 = ISA) */
    uint8_t  source;            /* Interrupt source */
    uint32_t gsi;               /* Global System Interrupt */
    uint16_t flags;             /* Flags */
} acpi_madt_iso_t;

/* ACPI Initialization Result */
typedef struct {
    int      valid;             /* ACPI tables found and valid */
    uint32_t revision;          /* ACPI revision (1.0, 2.0, etc.) */
    uint32_t cpu_count;         /* Number of CPUs detected */
    uint32_t lapic_addr;        /* Local APIC address */
    int      has_8259;          /* System has 8259 PIC */
    int      sci_interrupt;     /* SCI IRQ number */
} acpi_info_t;

/* Function Prototypes */
void acpi_init(void);
int acpi_find_rsdp(void);
int acpi_parse_tables(void);
int acpi_parse_fadt(void);
int acpi_parse_madt(void);
void acpi_get_info(acpi_info_t *info);
void acpi_print_info(void);

/* Diagnostics */
void acpi_dump_rsdp(void);

#endif /* _ACPI_H */
