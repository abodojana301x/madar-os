# 🎯 STAGE 5 - Phase 2: ACPI Basic Support - Implementation Plan

## 📋 Objective
Enable dynamic device discovery via ACPI (Advanced Configuration and Power Interface) instead of pure hardcoded PCI enumeration. This allows real hardware to be properly configured according to BIOS settings.

## 🎯 Goals
1. Parse RSDP (Root System Descriptor Pointer) from BIOS memory
2. Read ACPI tables (RSDT, XSDT, FADT, etc.)
3. Extract device information from ACPI namespace
4. Maintain backward compatibility with PCI enumeration
5. Add graceful fallback if ACPI unavailable

## 📐 Technical Architecture

### ACPI Table Structure
```
RSDP (0xE0000-0xFFFFF) → RSDT/XSDT → FADT, MADT, DSDT, etc.
                             ↓
                      Device Configuration
                             ↓
                      IRQ, Memory, I/O mappings
```

### ACPI Tables Needed (Phase 2 - Basic)
1. **RSDP** - Entry point descriptor
2. **RSDT** - Root System Descriptor Table (32-bit pointers)
3. **XSDT** - Extended System Descriptor Table (64-bit pointers)
4. **FADT** - Fixed ACPI Description Table (firmware info, power)
5. **MADT** - Multiple APIC Description Table (interrupt controller topology)

### Implementation Steps

#### Step 1: Create ACPI Header (include/acpi.h)
- RSDP structure definition
- ACPI table signatures and structures
- FADT structure (partial - just what we need)
- MADT structure and LAPIC/IOAPIC entries
- Error codes and status enumeration
- Function prototypes

#### Step 2: Create ACPI Implementation (kernel/acpi.c)
- RSDP search algorithm (scan 0xE0000-0xFFFFF)
- RSDP signature verification (8-byte signature + checksum)
- Table header validation
- FADT parsing
- MADT parsing for CPU and interrupt info
- Logging and diagnostics
- Fallback mechanism

#### Step 3: Integrate into Boot Flow (kernel/main.c)
- Add `acpi_init()` call in Layer 2 or Layer 3
- Populate device info from ACPI
- Log detected devices
- Fall back to defaults if ACPI unavailable

#### Step 4: Testing & Verification
- Build and verify no errors
- Run on QEMU (will likely not find ACPI in standard setup)
- Verify fallback behavior
- Check boot sequence

## 💻 Code Structure

### New Files
```
include/acpi.h       - ACPI type definitions and prototypes
kernel/acpi.c        - ACPI parsing implementation
```

### Modified Files
```
kernel/main.c        - Add acpi_init() call
include/types.h      - May add typedefs if needed
```

## 🔧 Key Functions

### acpi.h
```c
typedef struct {
    uint64_t revision;
    uint32_t rsdt_addr;
    uint64_t xsdt_addr;
    int is_valid;
} acpi_rsdp_t;

typedef struct {
    uint32_t cpu_count;
    uint32_t lapic_addr;
    int is_8259;
} acpi_madt_info_t;

void acpi_init(void);
int acpi_find_rsdp(void);
int acpi_parse_fadt(void);
int acpi_parse_madt(void);
void acpi_print_info(void);
```

### acpi.c
```c
static acpi_rsdp_t g_rsdp;
static acpi_madt_info_t g_madt_info;

void acpi_init(void) {
    // Search for RSDP
    // Parse RSDT/XSDT
    // Parse FADT
    // Parse MADT
    // Log results
}

// Checksum verification
static int acpi_checksum_valid(void *table, uint32_t length)

// Table search
static uint32_t* acpi_find_table(const char *signature)

// MADT parsing for CPU count, LAPIC address, etc.
```

## ✅ Verification Checklist

- [ ] Header file created with proper ACPI structures
- [ ] Implementation compiles without errors
- [ ] RSDP search algorithm implemented
- [ ] Checksum verification working
- [ ] Graceful fallback if ACPI not found
- [ ] Integration in kernel boot flow
- [ ] Boot sequence unbroken
- [ ] QEMU test successful
- [ ] Diagnostics show attempt to load ACPI (or fallback note)
- [ ] No breaking changes to existing code

## 📊 Success Criteria
✅ ACPI structures properly defined  
✅ RSDP search implemented (0xE0000-0xFFFFF)  
✅ Table parsing working (at least FADT, MADT)  
✅ Checksum validation implemented  
✅ Graceful fallback to defaults  
✅ System boots with or without ACPI  
✅ CPU and interrupt info extracted when available  

## ⏱️ Expected Duration
1-2 days for full implementation and testing

## 🎖️ Phase 2 Status: IN PROGRESS
