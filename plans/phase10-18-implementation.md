# Madar OS - Phases 10-18 Implementation Plan

## Phase 10: Storage, Filesystems, and Persistence

### Completed
- VFS basic operations (open, read, write, close, mkdir, readdir)
- Ramdisk with file creation
- FAT32 basic read support
- Initial disk I/O layer

### Remaining Work
1. **Strengthen VFS layering**: Add mount points, path resolution, device abstraction
2. **Persistent config storage**: Save settings to disk using INI-like format
3. **FAT32 write support**: Enable file creation, deletion, and modification on FAT32
4. **Mount lifecycle**: Proper mount/unmount with error handling
5. **File integrity checking**: Add basic checksums for file operations

### Implementation Notes
- Config persistence should write to a predefined path like `/system/config.ini`
- Use ramdisk as primary storage until real disk drivers mature
- FAT32 write operations must handle cluster allocation and directory entry updates

## Phase 11: Hardware and Driver Expansion

### Completed
- PCI bus scanning
- Framebuffer (GRUB-provided)
- PS/2 Keyboard and Mouse
- ATA (basic stub)
- RTL8139 (basic stub)

### Remaining Work
1. **PCI proper enumeration**: Full bus/device/function scan with class codes
2. **AHCI/SATA support**: Modern storage controller driver
3. **ACPI integration**: Parse DSDT for device discovery
4. **APIC/IOAPIC**: Replace PIC with modern interrupt controller
5. **USB controller (UHCI/EHCI)**: Keyboard/mouse via USB
6. **Driver capability matrix**: Document supported hardware

## Phase 12: Virtualization Targets

### Completed
- QEMU standard PC target
- VNC display output
- Serial console debugging

### Remaining Work
1. **QEMU with GTK/SDL display**: Better rendering performance
2. **VirtualBox guest additions**: VBDA, mouse integration
3. **VMware target**: VMware SVGA, VMXNET3
4. **Documentation**: Per-target display and performance characteristics

## Phase 13: Real Boot Media and Installer

### Completed
- ISO generation via grub-mkrescue
- GRUB bootloader configuration

### Remaining Work
1. **USB bootable workflow**: Script for dd to USB device
2. **Disk image generation**: raw, qcow2, vdi formats
3. **Partition expectations**: MBR/GPT documentation
4. **Live environment**: Boot to desktop without installation
5. **Installer**: Basic partition and copy workflow

## Phase 14: UEFI Boot Compatibility

### Planned
- Decide UEFI vs BIOS-only strategy
- EFI boot path with GOP framebuffer
- EFI system partition (ESP) support
- Memory map handoff differences
- Maintenance cost analysis

## Phase 15: Real Hardware Bring-Up

### Planned
- Target hardware selection (e.g., ThinkPad X220, QEMU, then real hardware)
- Serial logging checklist
- Fallback boot media
- Framebuffer detection testing
- Input and storage hardware testing
- Incompatibility documentation

## Phase 16: Testing and Continuous Validation

### Completed
- Build size verification
- Basic memory allocator test
- kprintf test
- Interrupt system test
- Layer 4 service tests
- Layer 6 GUI tests

### Remaining Work
1. **Boot regression tests**: Automated QEMU boot test
2. **Graphics sanity tests**: Verify resolution, color, and primitive rendering
3. **Input correctness tests**: Keyboard scancode validation, mouse packet validation
4. **Release checklists**: ISO build validation checklist
5. **Debug profiles**: Predefined QEMU debug launch configurations

## Phase 17: Performance and Responsiveness

### Planned
- Profile framebuffer copy operations (memcpy vs SSE/AVX)
- Profile primitive rendering hot paths
- Profile text rendering costs
- Profile blur/compositing (when implemented)
- Quality/performance mode toggle
- Benchmark baseline per release

## Phase 18: Release Engineering

### Planned
- Clean README with build and run instructions
- Release notes structure (CHANGELOG.md)
- Supported/unsupported feature matrix
- Screenshots and demo workflow
- Troubleshooting documentation
- CI/CD pipeline configuration
