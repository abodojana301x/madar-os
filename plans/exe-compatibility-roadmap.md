# Madar OS Professional EXE Compatibility Roadmap

## Purpose
This roadmap defines the path to support Windows `EXE` execution professionally inside Madar OS.

The goal is not to "fake-launch" a PE file, but to build a serious compatibility subsystem that can:
- load valid PE executables
- map memory correctly
- resolve imports
- provide essential Win32 behavior
- manage threads, TLS, exceptions, and synchronization
- support console and GUI application classes in phases

This plan assumes the current compatibility code in `compat/` is only an early foundation and needs major maturation.

---

## Success Definition

Professional EXE support means all of the following eventually work:
- real PE32+ loading
- multiple sections mapped with correct protections
- imports resolved correctly
- process and thread startup semantics are predictable
- heap, virtual memory, and TLS behave consistently
- common Win32 APIs are implemented or bridged correctly
- failures are traceable and debuggable
- basic console apps run first
- then simple GUI apps
- then increasingly complex apps with documented limits

---

## Phase 0: Scope, Target Matrix, and Compatibility Strategy

### Goals
- Define exactly what "EXE support" means for this project.
- Avoid promising full Windows compatibility too early.

### Work Items
1. Choose initial compatibility target:
   - `PE32+ x86_64` only first
   - no 32-bit WoW layer initially
2. Define supported application classes by stage:
   - console utilities
   - file utilities
   - simple native GUI apps
   - advanced GUI apps later
3. Define unsupported features early:
   - kernel drivers
   - anti-cheat
   - complex installer stacks
   - .NET unless runtime work is added later
4. Create a compatibility matrix:
   - executable type
   - subsystem
   - required DLLs
   - API families
5. Define whether the model is:
   - direct kernel-hosted compatibility
   - compatibility daemon
   - hybrid userspace subsystem

### Deliverables
- EXE compatibility scope document.
- Target application tiers.
- Supported vs unsupported matrix.

### Exit Criteria
- Team knows exactly what first-class EXE support means.

---

## Phase 1: PE Format and Loader Hardening

### Goals
- Make PE parsing and loading reliable, strict, and debuggable.

### Work Items
1. Harden DOS, COFF, and Optional Header parsing.
2. Validate section table thoroughly.
3. Enforce alignment rules:
   - section alignment
   - file alignment
   - image size
4. Support relocation parsing and application.
5. Support import directory parsing.
6. Support export lookup for DLL emulation layers.
7. Support TLS directory parsing.
8. Improve validation errors:
   - malformed header
   - overlapping sections
   - bad RVA
   - invalid relocation data
9. Add PE loader test corpus:
   - valid minimal PE
   - invalid PE
   - PE with imports
   - PE with relocations
   - PE with TLS

### Deliverables
- Production-grade PE parser/validator behavior.
- Clear PE diagnostics.

### Exit Criteria
- Loader can reject malformed binaries safely and explain why.
- Valid test binaries load consistently.

---

## Phase 2: Virtual Memory Mapping for PE Images

### Goals
- Map Windows images into Madar OS memory with correct layout and protection behavior.

### Work Items
1. Define image mapping model:
   - preferred image base
   - relocation if base unavailable
2. Implement proper section mapping by protection:
   - code execute/read
   - read-only data
   - read/write data
3. Reserve full image region before section population.
4. Zero-fill BSS-like regions correctly.
5. Implement guard handling and stack reservation policy.
6. Define virtual memory metadata for loaded images.
7. Track loaded image list for module lookup and debugging.

### Deliverables
- Stable image mapping subsystem.
- Correct per-section protections.

### Exit Criteria
- Loaded PE image matches expected memory layout.

---

## Phase 3: Process Model and EXE Runtime Container

### Goals
- Provide a clean runtime container for each loaded EXE.

### Work Items
1. Define compatibility process object:
   - pid
   - image base
   - module table
   - heap regions
   - thread list
   - handle table
   - environment block
2. Define startup sequence for EXE execution.
3. Define process teardown rules.
4. Add per-process compatibility state storage.
5. Define command-line and environment passing.
6. Add process-level diagnostics.

### Deliverables
- Managed EXE runtime container.

### Exit Criteria
- A loaded EXE runs inside a clearly defined process abstraction.

---

## Phase 4: Windows Thread Startup and TLS

### Goals
- Make thread startup semantics and TLS stable enough for real applications.

### Work Items
1. Define Windows-like thread creation path.
2. Support main thread initialization semantics.
3. Support secondary thread creation and join/wait.
4. Implement TLS allocation and access correctly.
5. Execute TLS callbacks at correct lifecycle points.
6. Ensure thread-local cleanup on exit.
7. Document thread entry conventions and ABI assumptions.

### Deliverables
- Stable threading for compatibility apps.

### Exit Criteria
- Multithreaded test EXEs behave predictably.

---

## Phase 5: Import Resolution and Module System

### Goals
- Resolve imported symbols professionally, not by scattered stubs alone.

### Work Items
1. Implement import descriptor walking.
2. Support import-by-name.
3. Support import-by-ordinal where needed.
4. Build a compatibility module registry for DLLs:
   - `kernel32`
   - `ntdll`
   - `user32`
   - `gdi32`
   - `advapi32`
   - `shell32`
   - others later
5. Implement export tables for internal compatibility DLL facades.
6. Add lazy vs eager import resolution policy.
7. Add diagnostics for missing imports.

### Deliverables
- Import resolver.
- Module registry and symbol lookup system.

### Exit Criteria
- Simple imported EXEs load without manual patching.

---

## Phase 6: Win32 Base API Layer

### Goals
- Provide the essential Win32 behavior expected by console and utility apps.

### Work Items
1. Strengthen `kernel32`-class APIs:
   - process APIs
   - thread APIs
   - file APIs
   - console APIs
   - memory APIs
   - environment APIs
   - timing APIs
2. Strengthen `ntdll`-adjacent low-level services as needed.
3. Define `GetLastError` and error propagation behavior.
4. Standardize handle model:
   - file handles
   - process handles
   - thread handles
   - event/mutex handles
5. Expand heap APIs.
6. Expand virtual memory APIs.

### Deliverables
- Reliable base API surface for non-GUI EXEs.

### Exit Criteria
- Basic console utilities can execute meaningful work.

---

## Phase 7: Filesystem, Path, and Handle Semantics

### Goals
- Make Windows-style path and file behavior predictable.

### Work Items
1. Define path translation:
   - `C:\...`
   - relative paths
   - current directory behavior
2. Map Windows path semantics onto Madar VFS.
3. Implement file attributes and metadata where required.
4. Implement access/share flag behavior at a practical level.
5. Improve handle lifetime tracking.
6. Add compatibility filesystem test cases.

### Deliverables
- Usable Windows-like file access layer.

### Exit Criteria
- File-oriented EXEs can open, read, write, and enumerate reliably.

---

## Phase 8: Exceptions, SEH, and Crash Behavior

### Goals
- Prevent many Windows programs from failing immediately on structured exception assumptions.

### Work Items
1. Formalize exception bridge behavior.
2. Implement basic SEH frame behavior where feasible.
3. Define mapping from Madar faults to compatibility exceptions.
4. Add process crash reporting with stack/module context.
5. Support safe termination paths.
6. Document which exception cases are unsupported initially.

### Deliverables
- Better survivability for apps expecting SEH-like behavior.

### Exit Criteria
- Known SEH smoke tests pass.

---

## Phase 9: Synchronization and IPC Compatibility

### Goals
- Support apps that expect common Windows synchronization primitives.

### Work Items
1. Implement event objects.
2. Implement mutexes.
3. Implement semaphores if needed.
4. Implement wait functions consistently.
5. Define timeout semantics.
6. Add named object namespace strategy if needed later.

### Deliverables
- Stable synchronization model for EXE processes.

### Exit Criteria
- Multi-threaded and wait-based test apps pass reliably.

---

## Phase 10: Console Application Tier

### Goals
- Reach the first truly useful milestone: real console EXE support.

### Work Items
1. Implement console stdin/stdout/stderr bridge.
2. Improve command-line parsing behavior.
3. Improve text encoding handling as needed.
4. Add sample supported apps:
   - hello world
   - file reader
   - environment dumper
   - thread test utility
5. Build a formal "console compatibility tier" checklist.

### Deliverables
- Tier 1 supported console EXE set.

### Exit Criteria
- A documented class of console EXEs runs successfully end-to-end.

---

## Phase 11: GUI Subsystem Bridge

### Goals
- Prepare support for GUI EXEs without claiming full Windows desktop compatibility too early.

### Work Items
1. Define `user32` compatibility layer direction.
2. Define window creation mapping into Madar window manager.
3. Define message loop model.
4. Implement basic window message dispatch:
   - create
   - paint
   - input
   - close
5. Define cursor, focus, and activation behavior.
6. Build minimum GUI test apps.

### Deliverables
- Basic GUI EXE compatibility architecture.

### Exit Criteria
- Simple native windowed EXEs can create a visible window and process basic events.

---

## Phase 12: GDI and Drawing Compatibility

### Goals
- Support simple GUI rendering expected by native Windows desktop apps.

### Work Items
1. Define minimum viable `gdi32` support.
2. Implement device context abstraction.
3. Implement text output primitives.
4. Implement rectangle, line, brush, pen basics.
5. Map GDI drawing to Madar rendering engine.
6. Document unsupported advanced drawing paths.

### Deliverables
- Basic GDI compatibility for simple GUI apps.

### Exit Criteria
- Minimal GUI apps can draw text and simple controls.

---

## Phase 13: Loader Support for DLLs

### Goals
- Move from static API facades toward a proper runtime module model.

### Work Items
1. Support loading companion DLL images.
2. Support module dependency graph.
3. Support DLL entry lifecycle.
4. Support `GetModuleHandle` and `GetProcAddress` robustly.
5. Add DLL unload policy.

### Deliverables
- Runtime DLL loader.

### Exit Criteria
- EXEs with one or more dependent DLLs can load through a documented path.

---

## Phase 14: Security and Sandbox Maturity

### Goals
- Keep EXE execution safe and observable.

### Work Items
1. Mature path sandbox policy.
2. Add memory ceilings per process.
3. Add CPU/watchdog controls.
4. Add syscall filtering policy where possible.
5. Add logging and audit events.
6. Define trust levels:
   - internal test apps
   - signed known apps
   - untrusted apps

### Deliverables
- Safer EXE runtime.

### Exit Criteria
- EXE execution is controlled, monitored, and fail-safe.

---

## Phase 15: Debugging, Tracing, and Developer Tooling

### Goals
- Make compatibility failures diagnosable.

### Work Items
1. Add loader trace levels.
2. Add import resolution logs.
3. Add API call tracing.
4. Add structured crash reports.
5. Add module and handle dump tools.
6. Add compatibility test harness commands.

### Deliverables
- Debuggable compatibility subsystem.

### Exit Criteria
- When an EXE fails, developers can identify where and why.

---

## Phase 16: Compatibility Test Program Suite

### Goals
- Build confidence through repeatable tests instead of anecdotal launches.

### Work Items
1. Create internal EXE fixture suite:
   - minimal PE
   - imports
   - threads
   - TLS
   - file I/O
   - console I/O
   - exceptions
   - GUI create/show
2. Add pass/fail reports.
3. Version the expected compatibility baseline.
4. Add regression tracking.

### Deliverables
- Compatibility conformance suite.

### Exit Criteria
- Progress is measured by stable test tiers, not guesswork.

---

## Phase 17: Application Tier Progression

### Goals
- Move from synthetic tests to real-world programs in controlled layers.

### Work Items
1. Tier A:
   - trivial console apps
   - file utilities
2. Tier B:
   - more realistic CLI tools
   - config utilities
3. Tier C:
   - simple Win32 GUI apps
4. Tier D:
   - richer native apps with more DLL and GDI expectations
5. Maintain per-app compatibility notes:
   - works
   - partial
   - fails
   - blocked by missing APIs

### Deliverables
- Real application compatibility matrix.

### Exit Criteria
- The project can show which types of EXEs truly work.

---

## Phase 18: Performance and Memory Efficiency

### Goals
- Ensure compatibility does not become too slow or memory-heavy.

### Work Items
1. Profile image loading.
2. Profile import resolution.
3. Profile API dispatch overhead.
4. Profile GUI bridge cost for EXE windows.
5. Optimize hot paths carefully after correctness.

### Deliverables
- Measured performance baseline.

### Exit Criteria
- Supported EXEs run with acceptable responsiveness for their tier.

---

## Phase 19: Documentation and Public Support Policy

### Goals
- Present EXE support honestly and professionally.

### Work Items
1. Publish supported EXE classes.
2. Publish unsupported features.
3. Publish known missing DLL/API families.
4. Publish test workflow for adding new sample apps.
5. Publish debugging guide for failed EXEs.

### Deliverables
- Honest compatibility documentation.

### Exit Criteria
- Users understand what "EXE support" means in Madar OS.

---

## Recommended Implementation Order

1. Phase 0: Scope and strategy
2. Phase 1: PE loader hardening
3. Phase 2: Memory mapping
4. Phase 3: Process runtime container
5. Phase 4: Threads and TLS
6. Phase 5: Import resolution and module registry
7. Phase 6: Base Win32 APIs
8. Phase 7: Filesystem and handle semantics
9. Phase 8: Exceptions and SEH
10. Phase 9: Synchronization
11. Phase 10: Console EXE milestone
12. Phase 11: GUI subsystem bridge
13. Phase 12: GDI basics
14. Phase 13: DLL loader maturity
15. Phase 14: Sandbox and safety
16. Phase 15: Debugging and tracing
17. Phase 16: Test suite
18. Phase 17: Real app tiers
19. Phase 18: Performance tuning
20. Phase 19: Documentation and support policy

---

## Practical Near-Term Milestone Plan

### Milestone 1
- Harden PE parsing
- Harden image mapping
- Improve process/thread/TLS startup

### Milestone 2
- Resolve imports
- Implement reliable `kernel32`-class file, memory, and console APIs
- Run simple console EXEs

### Milestone 3
- Improve SEH, synchronization, and diagnostics
- Expand supported console programs

### Milestone 4
- Add `user32` message loop bridge
- Add basic `gdi32`
- Run simple GUI EXEs

### Milestone 5
- DLL loading maturity
- Safer sandboxing
- Broader compatibility matrix

---

## Definition of "Professional" in This Plan

In this roadmap, professional EXE support means:
- predictable behavior
- proper subsystem architecture
- repeatable compatibility testing
- documented support boundaries
- debug visibility
- gradual expansion by application tier

It does not mean "claim full Windows compatibility quickly." It means building a compatibility layer with discipline and measurable progress.
