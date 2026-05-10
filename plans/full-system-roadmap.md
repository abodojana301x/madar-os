# Madar OS Full System Roadmap

## Purpose
This roadmap defines the full path to evolve Madar OS from its current educational QEMU-based state into a more complete operating system that boots, renders, and behaves closer to mainstream desktop systems.

The roadmap is intentionally staged. Each phase has a clear purpose, deliverables, dependencies, risks, and exit criteria.

---

## Phase 0: Baseline, Audit, and Architecture Freeze

### Goals
- Establish the current technical baseline.
- Document what already works and what is stubbed, incomplete, or misleading.
- Freeze short-term architecture decisions before large refactors begin.

### Work Items
1. Audit the boot flow from GRUB to `kernel_main`.
2. Audit memory management, interrupt handling, scheduler, framebuffer, and input paths.
3. Audit graphics resolution assumptions, font limitations, and rendering quality bottlenecks.
4. Audit build targets for ISO, QEMU, debugging, and test workflows.
5. Audit which drivers are real, partial, or placeholders.
6. Produce a component map:
   - boot
   - kernel core
   - drivers
   - services
   - GUI
   - desktop shell
   - apps
7. Define short-term architectural constraints:
   - BIOS first or UEFI next
   - 64-bit only or mixed compatibility
   - framebuffer-first GUI or future GPU abstraction

### Deliverables
- System architecture document.
- Boot path diagram.
- Driver maturity table.
- Graphics limitations report.
- Known blockers list.

### Exit Criteria
- Team can explain exactly how the system boots and where the weak points are.
- No major subsystem remains undocumented.

---

## Phase 1: Boot and Platform Reliability

### Goals
- Make boot behavior deterministic and easier to debug.
- Strengthen the platform layer so the kernel enters a stable execution environment every time.

### Work Items
1. Harden the Multiboot2 boot path.
2. Validate all framebuffer tags and fallback behavior.
3. Verify page table setup and memory mapping assumptions.
4. Add clearer boot logging for:
   - framebuffer detection
   - memory map parsing
   - long mode transition
   - kernel handoff
5. Review stack setup and alignment guarantees.
6. Separate boot-time failures into identifiable error states.
7. Add boot assertions where possible.
8. Add a minimal boot diagnostics screen for fatal failures.

### Deliverables
- Reliable GRUB boot flow.
- Better boot diagnostics.
- Reproducible boot logs.

### Risks
- Hidden assumptions in identity mapping.
- Early boot debugging remains difficult without serial-first discipline.

### Exit Criteria
- Boot works consistently in QEMU.
- Common boot failures are observable and diagnosable.

---

## Phase 2: Interrupts, Timing, and Input Correctness

### Goals
- Move from partially wired input to a robust interrupt and input architecture.
- Eliminate fragile behavior in keyboard and mouse handling.

### Work Items
1. Review IDT coverage and actual ISR/IRQ wiring.
2. Ensure `sti`, PIC unmasking, and handler registration are in the correct lifecycle.
3. Clarify which devices use interrupts and which still require polling.
4. Refactor keyboard input path:
   - scancode handling
   - press/release correctness
   - modifier keys
   - buffer integrity
5. Refactor mouse input path:
   - PS/2 initialization
   - packet synchronization
   - movement scaling
   - click state transitions
6. Add input tracing for debugging.
7. Stabilize timer tick behavior and scheduler interaction.
8. Define a clean input abstraction for GUI consumers.

### Deliverables
- Stable keyboard input.
- Stable mouse input.
- Documented IRQ lifecycle.
- Clear separation between raw input and GUI events.

### Exit Criteria
- Input works predictably without hacks.
- GUI no longer depends on accidental polling behavior for correctness.

---

## Phase 3: Memory, Process, and Core Kernel Hardening

### Goals
- Improve the integrity of the kernel core before investing further in higher-level features.

### Work Items
1. Review heap safety and failure handling.
2. Audit scheduler behavior and thread lifecycle.
3. Review process abstraction and syscall correctness.
4. Improve synchronization primitives and contention handling.
5. Add kernel diagnostics for:
   - allocations
   - process count
   - thread state
   - timer statistics
6. Add assertions and defensive checks in critical core paths.
7. Define crash-handling and panic-reporting conventions.

### Deliverables
- More stable core runtime.
- Better observability for kernel state.

### Exit Criteria
- Kernel survives longer stress loops.
- No obvious corruption under normal GUI use.

---

## Phase 4: Graphics Foundation Rebuild

### Goals
- Fix the real cause of the ugly, pixelated desktop.
- Replace the current fragile rendering baseline with something suitable for a modern shell.

### Work Items
1. Raise internal rendering resolution:
   - target 1600x900 first
   - optional 1920x1080 path next
2. Align GUI surface dimensions with framebuffer mode.
3. Improve framebuffer copy path efficiency.
4. Refactor drawing primitives for quality:
   - cleaner lines
   - smoother rounded corners
   - better circles
   - cleaner borders
5. Implement real alpha blending.
6. Build a compositing model for layered surfaces.
7. Prepare the engine for blur and translucency.
8. Add quality modes if performance becomes an issue.

### Deliverables
- Higher-resolution rendering.
- Cleaner primitive rendering.
- Proper transparency groundwork.

### Exit Criteria
- Visible reduction in pixelation.
- Edges and shapes look intentionally rendered, not blocky by default.

---

## Phase 5: Font and Text System Overhaul

### Goals
- Replace the tiny bitmap font with a readable text system.
- Lay the groundwork for Arabic-first computing.

### Work Items
1. Replace the 5x7 placeholder font path.
2. Introduce larger bitmap fonts first for quick visual improvement.
3. Add font metrics support:
   - width
   - height
   - line spacing
   - baseline
4. Introduce scalable font rendering if feasible.
5. Add text alignment support:
   - left
   - center
   - right
6. Improve clipping and text placement.
7. Prepare for Arabic shaping and bidirectional text handling.
8. Integrate typography rules with UI widgets and window chrome.

### Deliverables
- Clean readable Latin UI text.
- Text layout infrastructure for Arabic support.

### Exit Criteria
- UI text no longer looks toy-like.
- Labels, titles, and menus remain readable at higher resolutions.

---

## Phase 6: Window Compositor and Modern Visual System

### Goals
- Build a proper modern desktop rendering model instead of painting raw rectangles.

### Work Items
1. Introduce layered window surfaces.
2. Implement proper shadows and depth.
3. Add translucent panels and surfaces.
4. Implement background blur for glass effects.
5. Improve active/inactive window states.
6. Add cleaner control buttons and chrome behaviors.
7. Improve hit-testing around rounded edges.
8. Define a visual language:
   - spacing
   - radius
   - shadows
   - highlights
   - elevation

### Deliverables
- Convincing glass-style taskbar and windows.
- Modern depth and layering.

### Exit Criteria
- Windows look composed, not merely drawn.
- Glass effects are visually consistent and technically real.

---

## Phase 7: Desktop Shell Redesign

### Goals
- Redesign the shell only after the renderer is good enough to support it.

### Work Items
1. Rebuild taskbar layout.
2. Rebuild Start menu structure.
3. Add centered launcher cluster option.
4. Add search box and pinned apps grid.
5. Improve session tray and clock area.
6. Add hover, pressed, and active states.
7. Improve wallpaper/background system.
8. Add animation guidelines for opening menus and windows.

### Deliverables
- A coherent modern shell.
- Visual direction closer to current desktop systems.

### Exit Criteria
- The shell feels intentional and navigable.
- Core shell surfaces are visually consistent.

---

## Phase 8: UI Toolkit and Application Framework

### Goals
- Stop building interfaces through repeated hardcoded coordinate logic.

### Work Items
1. Define a reusable widget framework.
2. Add layout primitives:
   - stack
   - row
   - column
   - grid
   - padding
   - alignment
3. Add reusable controls:
   - buttons
   - text fields
   - lists
   - menus
   - dialogs
   - toggles
4. Define theming hooks.
5. Improve event propagation and focus handling.
6. Standardize app window creation and chrome integration.

### Deliverables
- Internal UI toolkit.
- Less duplicated drawing logic.

### Exit Criteria
- New apps can be built faster and look consistent.

---

## Phase 9: Native App Maturity

### Goals
- Turn demos and placeholders into coherent desktop applications.

### Work Items
1. Terminal:
   - better text rendering
   - scrolling
   - selection
   - ANSI basics
2. Files:
   - list/grid modes
   - navigation
   - icons
   - actions
3. Settings:
   - display
   - theme
   - network
   - language
4. Task manager:
   - process list
   - stats
   - kill flow
5. Browser/web view:
   - clarify scope
   - keep minimal if full browser is out of scope

### Deliverables
- Usable core desktop apps.

### Exit Criteria
- The system has a believable minimum desktop app suite.

---

## Phase 10: Storage, Filesystems, and Persistence

### Goals
- Move from temporary/demo behavior toward persistence expected from a real OS.

### Work Items
1. Strengthen VFS layering.
2. Improve ramdisk semantics and testing.
3. Improve FAT32 support and reliability.
4. Add persistent user/config storage strategy.
5. Define mount lifecycle and failure behavior.
6. Add file integrity and error reporting.

### Deliverables
- More reliable file operations.
- Persistent configuration path.

### Exit Criteria
- Users can change settings and keep them across boots.

---

## Phase 11: Hardware and Driver Expansion

### Goals
- Reduce the gap between a QEMU demo and a real bootable OS.

### Work Items
1. Improve framebuffer compatibility paths.
2. Improve keyboard and mouse support beyond the narrow current assumptions.
3. Review PCI discovery quality.
4. Improve disk controller support.
5. Improve network device handling.
6. Plan ACPI and APIC adoption if required.
7. Define a driver capability matrix.

### Deliverables
- Better virtual hardware support.
- Clear path toward broader compatibility.

### Exit Criteria
- The OS behaves reliably across more than one virtual hardware profile.

---

## Phase 12: Virtualization Targets Beyond Current QEMU Flow

### Goals
- Run the OS in more realistic and less visually degraded environments.

### Work Items
1. Keep QEMU as the main development target.
2. Improve local GUI launch path beyond browser VNC when available.
3. Add optional targets for:
   - QEMU with GTK/SDL
   - VirtualBox
   - VMware
4. Document guest display limitations per target.
5. Define a recommended developer runtime matrix.

### Deliverables
- Better dev and demo experience.

### Exit Criteria
- The OS can be tested in at least two practical VM environments.

---

## Phase 13: Real Boot Media and Installer Path

### Goals
- Make the system bootable from media in a more realistic OS distribution model.

### Work Items
1. Produce stable ISO releases.
2. Add disk image generation:
   - raw image
   - bootable image
3. Add USB-bootable workflow documentation.
4. Define partition expectations.
5. Plan installer approach:
   - live environment first
   - installer second
6. Add release packaging conventions.

### Deliverables
- Release ISO.
- Disk image workflow.
- USB boot instructions.

### Exit Criteria
- A tester can boot the OS from prepared media without custom handholding.

---

## Phase 14: UEFI and Broader Boot Compatibility

### Goals
- Expand beyond a BIOS/GRUB-only comfort zone if the project intends broader hardware realism.

### Work Items
1. Decide whether UEFI is in scope.
2. If yes:
   - define EFI boot path
   - define loader strategy
   - define memory handoff model
3. Compare maintenance cost of BIOS-only vs BIOS+UEFI.

### Deliverables
- Boot compatibility strategy document.

### Exit Criteria
- Project has a conscious, documented boot strategy.

---

## Phase 15: Real Hardware Bring-Up

### Goals
- Move carefully from virtual machines to selected physical machines.

### Work Items
1. Choose a narrow hardware test target first.
2. Prepare safe bring-up checklist:
   - serial logging
   - fallback boot media
   - debug build
3. Test framebuffer detection on hardware.
4. Test input and storage on hardware.
5. Document incompatibilities and failure modes.

### Deliverables
- First real-hardware boot report.
- Supported hardware shortlist.

### Exit Criteria
- The OS reaches a controlled desktop or diagnostics mode on at least one real machine.

---

## Phase 16: Testing, Tooling, and Continuous Validation

### Goals
- Prevent regressions while the system grows.

### Work Items
1. Expand build verification.
2. Add subsystem smoke tests.
3. Add boot regression tests in QEMU.
4. Add graphics sanity tests.
5. Add input correctness tests.
6. Add release checklists for ISO builds.
7. Add debug profiles and log presets.

### Deliverables
- Reproducible validation workflow.

### Exit Criteria
- New changes break less often and regressions are caught earlier.

---

## Phase 17: Performance and Responsiveness

### Goals
- Ensure quality improvements do not make the system unreasonably slow.

### Work Items
1. Profile framebuffer copying.
2. Profile primitive rendering hot paths.
3. Profile text rendering costs.
4. Profile blur/compositing cost.
5. Add optional quality/performance modes.

### Deliverables
- Performance baseline per release.

### Exit Criteria
- The desktop remains usable after visual improvements.

---

## Phase 18: Release Engineering and Public Usability

### Goals
- Make the project understandable and usable by others.

### Work Items
1. Clean documentation.
2. Add release notes structure.
3. Add supported/unsupported feature matrix.
4. Add screenshots and demo workflow.
5. Add troubleshooting docs.

### Deliverables
- Public-facing release package.

### Exit Criteria
- Someone new can build, boot, and evaluate the system with reasonable guidance.

---

## Recommended Implementation Order

1. Phase 0: Baseline and audit
2. Phase 1: Boot reliability
3. Phase 2: Interrupts and input correctness
4. Phase 3: Core kernel hardening
5. Phase 4: Graphics foundation rebuild
6. Phase 5: Font and text overhaul
7. Phase 6: Compositor and visual system
8. Phase 7: Shell redesign
9. Phase 8: UI toolkit
10. Phase 9: Apps
11. Phase 10: Persistence and filesystems
12. Phase 11: Driver expansion
13. Phase 12: Better VM targets
14. Phase 13: Bootable media workflow
15. Phase 14: UEFI strategy
16. Phase 15: Real hardware bring-up
17. Phase 16: Testing and validation
18. Phase 17: Performance tuning
19. Phase 18: Release engineering

---

## Near-Term Priority Set

The most important practical sequence for visible progress is:

1. Fix boot and input correctness.
2. Rebuild graphics resolution and text rendering.
3. Add real alpha blending and improve primitive quality.
4. Then redesign the desktop visually.

This order avoids spending time polishing a renderer that cannot yet produce polished output.
