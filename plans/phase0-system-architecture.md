# Madar OS - Phase 0: System Architecture Document

## 1. System Overview

Madar OS is an x86-64 hobby operating system with:
- **Boot**: GRUB/Multiboot2 → Long Mode (64-bit) → Kernel entry
- **Kernel Core**: Physical memory management, heap, interrupts (IDT/PIC), scheduler (round-robin), processes, syscalls
- **Drivers**: PCI, framebuffer, PS/2 keyboard/mouse, ATA, RTL8139 NIC
- **Services**: VFS, ramdisk, FAT32, IPC (pipes), signals, security (UID/GID), network stack (ARP/IP/ICMP/TCP/UDP)
- **Compatibility**: PE32/PE32+ loader, Win32 API stubs, TLS, SEH, syscall bridge
- **GUI**: Double-buffered framebuffer graphics, font engine (5x7 bitmap), window manager, widgets, taskbar, themes (dark/light), RTL support, animations
- **Desktop**: Shell, panel, start menu, app registry, session management
- **Apps**: File manager, terminal, settings, web browser, task manager, clipboard

## 2. Boot Path Diagram

```
GRUB (BIOS)
  │
  ├── Multiboot2 header (0xE85250D6)
  │   ├── Arch: i386 (0)
  │   ├── Framebuffer tag: 1024x768x32
  │   └── End tag
  │
  ├── GRUB loads kernel.bin at 1MB
  │
  ├── boot_start (kernel/boot.s)
  │   ├── cli
  │   ├── Check Multiboot2 magic (0x36D76289)
  │   ├── Save magic + mbi pointer
  │   ├── Setup stack (32KB)
  │   ├── setup_page_tables:
  │   │   ├── Clear PML4, PDPT, PD tables (6 x 4KB)
  │   │   ├── PML4[0] → PDPT
  │   │   ├── PDPT[0..3] → PD0..PD3
  │   │   └── Identity map first 4 GiB (2MiB pages)
  │   ├── enable_long_mode:
  │   │   ├── CR4.PAE = 1
  │   │   ├── CR3 → PML4
  │   │   ├── MSR 0xC0000080 → EFER.LME = 1
  │   │   └── CR0.PG = 1
  │   ├── LGDT → load GDT64
  │   └── far jump → long_mode_entry (0x08)
  │
  ├── long_mode_entry (code64)
  │   ├── Set DS/ES/SS/FS/GS = 0x10
  │   ├── RSP = stack_top (16-byte aligned)
  │   ├── RBP = 0
  │   ├── EDI = magic, ESI = mbi pointer
  │   └── call kernel_main
  │
  └── kernel_main (kernel/main.c)
      ├── serial_init()      → COM1 @ 115200
      ├── kprintf_init()     → Unified printf
      ├── memory_init()      → 128MB, page allocator
      ├── fb_init_from_multiboot() → Framebuffer detection
      ├── idt_init()         → IDT setup
      ├── pic_init()         → PIC remap (IRQ0-15 → INT32-47)
      ├── layer2_init():
      │   ├── acpi_init()
      │   ├── heap_init()    → Kernel heap
      │   ├── process_init()
      │   ├── scheduler_init()
      │   ├── timer_init()   → PIT at 100Hz
      │   ├── syscall_init()
      │   └── kernel_diag_print_snapshot()
      ├── layer3_init():     → Drivers (PCI, FB, KBD, MOUSE, ATA, RTL8139)
      ├── layer4_init():     → Services (VFS, FAT32, IPC, NET, SECURITY)
      ├── layer5_init():     → Compatibility (PE loader)
      ├── layer6_init():     → GUI (GFX, WM, widgets, taskbar)
      └── layer7_init():     → Desktop shell + apps
```

## 3. Component Map

| Component | Status | Location | Lines |
|-----------|--------|----------|-------|
| **Boot** | ✅ Done | kernel/boot.s | 201 |
| **Memory Manager** | ✅ Basic | kernel/memory.c | 94 |
| **Heap** | ✅ Basic | kernel/heap.c | - |
| **IDT** | ✅ Basic | kernel/idt.c | 44 |
| **PIC** | ✅ Basic | kernel/pic.c | - |
| **Serial** | ✅ Done | kernel/serial.c | - |
| **VGA** | ✅ Done | kernel/vga.c | - |
| **kprintf** | ✅ Done | kernel/kprintf.c | - |
| **String** | ✅ Done | kernel/string.c | - |
| **Scheduler** | ⚠️ Partial (needs context switch) | kernel/scheduler.c | 149 |
| **Process/Thread** | ⚠️ Partial | kernel/process.c | - |
| **Syscall** | ⚠️ Partial | kernel/syscall.c | - |
| **Timer** | ✅ Basic | kernel/timer.c | - |
| **ACPI** | ⚠️ Stub | kernel/acpi.c | - |
| **Panic** | ✅ Basic | kernel/panic.c | - |
| **Spinlock** | ✅ Done | kernel/spinlock.c | - |
| **Sync/Atomic** | ⚠️ Basic | kernel/sync.c | - |
| **PCI** | ⚠️ Partial | drivers/pci.c | - |
| **Framebuffer** | ⚠️ Needs improvement | drivers/fb.c | 227 |
| **Keyboard** | ✅ Basic | drivers/kbd.c | 102 |
| **Mouse** | ✅ Basic | drivers/mouse.c | 183 |
| **ATA** | ⚠️ Stub | drivers/ata.c | - |
| **RTL8139** | ⚠️ Stub | drivers/rtl8139.c | - |
| **IRQ Manager** | ✅ Basic | drivers/irq_manager.c | - |
| **VFS** | ✅ Basic | services/vfs.c | - |
| **Ramdisk** | ✅ Basic | services/ramdisk.c | - |
| **FAT32** | ⚠️ Partial | services/fat32.c | - |
| **IPC** | ✅ Basic | services/ipc.c | - |
| **Signals** | ⚠️ Partial | services/signal.c | - |
| **Security** | ✅ Basic | services/security.c | - |
| **Network Stack** | ⚠️ Partial | services/*.c | - |
| **GUI Backend** | ⚠️ Needs optimization | gui/gfx_backend.c | 88 |
| **Font Engine** | ❌ 5x7 bitmap (tiny) | gui/font_engine.c | 88 |
| **Window Manager** | ⚠️ Basic | gui/wm_core.c | 217 |
| **Widgets** | ⚠️ Basic | gui/widgets.c | - |
| **Theme** | ✅ Good (dark/light) | gui/theme.c | 262 |
| **RTL Support** | ⚠️ Basic (Arabic) | gui/rtl_support.c | 84 |
| **UI Toolkit** | ⚠️ Basic | gui/ui_toolkit.c | 302 |
| **Desktop Shell** | ⚠️ Basic | desktop/shell.c | 85 |
| **Panel** | ⚠️ Basic | desktop/panel.c | - |
| **Start Menu** | ⚠️ Basic | desktop/start_menu.c | - |
| **App Registry** | ✅ Basic | desktop/app_registry.c | - |
| **PE Loader** | ⚠️ Basic | compat/pe_loader.c | - |
| **Win32 Stubs** | ⚠️ Partial | compat/win32_stubs.c | - |
| **Build System** | ✅ Done | Makefile | 128 |
| **Linker Script** | ✅ Done | linker.ld | 71 |

## 4. Driver Maturity Table

| Driver | Initialization | Functionality | Interrupt | DMA | Error Handling |
|--------|---------------|---------------|-----------|-----|----------------|
| **Framebuffer** | ✅ GRUB provides | ✅ Basic draw | N/A | N/A | ⚠️ Minimal |
| **PS/2 Keyboard** | ✅ Complete | ✅ Scancode → char | ✅ IRQ1 | N/A | ⚠️ Basic |
| **PS/2 Mouse** | ✅ Complete | ✅ 3-byte packets | ✅ IRQ12 | N/A | ⚠️ Basic |
| **PCI** | ⚠️ Basic scan | ⚠️ Partial discovery | N/A | N/A | ❌ None |
| **ATA** | ⚠️ Stub | ❌ Not functional | ⚠️ Stub | ❌ | ❌ None |
| **RTL8139** | ⚠️ Stub | ❌ Not functional | ⚠️ Stub | ❌ | ❌ None |

## 5. Graphics Limitations Report

| Issue | Severity | Description |
|-------|----------|-------------|
| Low resolution | HIGH | Only 1024x768, needs 1600x900+ |
| Tiny bitmap font | HIGH | 5x7 pixel font is unreadable at higher res |
| No alpha blending | HIGH | `gfx_pixel_blend()` is a stub |
| No compositor | HIGH | Windows draw directly, no layering |
| No real shadows | MEDIUM | Shadow is a hack, not real blur |
| Slow framebuffer copy | MEDIUM | Full buffer copy every frame |
| No vsync | LOW | No vertical sync |
| No GPU abstraction | LOW | Only linear framebuffer |

## 6. Known Blockers List

| Blocker | Phase | Priority | Description |
|---------|-------|----------|-------------|
| No real context switch | Phase 2 | HIGH | scheduler uses RSP hack |
| No user mode (Ring 3) | Phase 3 | HIGH | Everything runs in Ring 0 |
| No virtual memory | Phase 3 | HIGH | Identity mapping only |
| No dynamic resolution switching | Phase 4 | MEDIUM | GRUB sets resolution |
| Small font at high res | Phase 5 | HIGH | 5x7 bitmap too small |
| No Arabic glyph shaping | Phase 5 | MEDIUM | UTF-8 not properly handled |
| No compositing | Phase 6 | HIGH | No layered surfaces |
| No persistent storage | Phase 10 | MEDIUM | Ramdisk only |

## 7. Short-Term Architectural Constraints

- **Boot**: BIOS/GRUB first, UEFI planned for Phase 14
- **Architecture**: 64-bit only (long mode)
- **Graphics**: Framebuffer-first, GPU abstraction deferred
- **Memory**: Identity mapping (no virtual memory yet)
- **Execution**: All Ring 0 (user mode deferred)
