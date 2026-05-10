# Madar OS - نظام تشغيل مَدَار

A professional x86-64 hobby operating system with modern GUI, networking, and Windows PE compatibility.

## Quick Start

```bash
make clean      # Clean build artifacts
make test       # Build and verify
make run        # Run in QEMU (serial console)
make run-gui    # Run with GUI display
```

## System Requirements

- **Build**: GCC, GNU Make, QEMU (optional for running)
- **Runtime**: x86-64 machine or emulator (QEMU, VirtualBox)
- **Memory**: 256 MB minimum

## Architecture

```
┌─────────────────────────────────────────────┐
│ Layer 7: Apps (FileMgr, Settings, Term, Web)│
│ Layer 7: Desktop Shell (Panel, Start Menu)  │
│ Layer 6: GUI (WM, Compositor, Widgets)      │
│ Layer 5: Compatibility (PE Loader, Win32)   │
│ Layer 4: Services (VFS, FAT32, IPC, Net)    │
│ Layer 3: Drivers (PCI, FB, KBD, MOUSE, ATA)│
│ Layer 2: Kernel Core (Sched, Heap, Process) │
│ Layer 1: HAL (Boot, Memory, IDT, PIC)       │
├─────────────────────────────────────────────┤
│ GRUB Bootloader → Long Mode (x86-64)        │
└─────────────────────────────────────────────┘
```

## Key Features

- **64-bit Long Mode** with identity-mapped 4GB address space
- **Multiboot2** compliant boot via GRUB
- **Framebuffer GUI** at 1600x900 with ARGB alpha blending
- **Window Manager** with compositing, shadows, glass effects
- **8x13 Font Engine** with RTL/Arabic text support
- **PS/2 Keyboard** with full scancode translation and modifier keys
- **PS/2 Mouse** with acceleration curve
- **Round-Robin Scheduler** with thread management
- **Kernel Heap** with bitmap-based allocation
- **VFS** with ramdisk and FAT32 support
- **TCP/IP Stack** (ARP, IP, ICMP, UDP)
- **Windows PE Loader** (experimental)
- **Dark/Light Themes**

## Project Structure

```
kernel/         Kernel core (boot, memory, scheduler, syscalls)
drivers/        Hardware drivers (PCI, FB, KBD, MOUSE, ATA, NET)
services/       System services (VFS, FAT32, IPC, Net, Security)
compat/         Windows PE compatibility layer
gui/            Graphics system (compositor, WM, widgets, fonts)
desktop/        Desktop shell (panel, start menu, session)
apps/           User applications (fileman, settings, taskmgr, web)
system/         System init and boot sequence
include/        Common headers
build/          Build output directory
plans/          Roadmap and architecture documentation
```

## Roadmap Status

| Phase | Description | Status |
|-------|-------------|--------|
| 0 | Baseline & Architecture Freeze | ✅ Complete |
| 1 | Boot Platform Reliability | ✅ Complete |
| 2 | Interrupts & Input Correctness | ✅ Complete |
| 3 | Core Kernel Hardening | ✅ Complete |
| 4 | Graphics Foundation Rebuild | ✅ Complete |
| 5 | Font & Text System Overhaul | ✅ Complete |
| 6 | Window Compositor & Visuals | ✅ Complete |
| 7 | Desktop Shell Redesign | ✅ Complete |
| 8 | UI Toolkit & App Framework | ✅ Complete |
| 9 | Native App Maturity | ✅ Complete |
| 10 | Storage & Filesystem Persistence | 📋 Planned |
| 11 | Hardware & Driver Expansion | 📋 Planned |
| 12 | Virtualization Targets | 📋 Planned |
| 13 | Boot Media & Installer | 📋 Planned |
| 14 | UEFI Compatibility | 📋 Planned |
| 15 | Real Hardware Bring-Up | 📋 Planned |
| 16 | Testing & CI | 📋 Planned |
| 17 | Performance Optimization | 📋 Planned |
| 18 | Release Engineering | 📋 Planned |

See [plans/full-system-roadmap.md](plans/full-system-roadmap.md) for details.
