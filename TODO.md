# Madar OS - MDE Stage 2 (Window Manager v0.1)

## 📌 Phase 1 (نفذ المرحلة 1): Boot and Platform Reliability

- [ ] Harden the Multiboot2 boot path
- [ ] Validate all framebuffer tags and fallback behavior
- [ ] Verify page table setup and memory mapping assumptions
- [ ] Add clearer boot logging for:
  - [ ] framebuffer detection
  - [ ] memory map parsing
  - [ ] long mode transition
  - [ ] kernel handoff
- [ ] Review stack setup and alignment guarantees
- [ ] Separate boot-time failures into identifiable error states
- [ ] Add boot assertions where possible
- [ ] Add a minimal boot diagnostics screen for fatal failures

**Exit criteria (Phase 1):**
- [ ] Boot works consistently in QEMU
- [ ] Common boot failures are observable and diagnosable




## ✅ Already Implemented


Based on code analysis, the following are already working:

1. **Window Structure** (`gui/wm_core.h:wm_window_t`)
   - id, x, y, w, h, z_index, state, title

2. **Window List** (`gui/wm_core.c:wm_create`)
   - MAX_WINDOWS = 64
   - Window creation with id

3. **Window Rendering** (`gui/wm_core.c:wm_draw_all`)
   - Renders all windows in z-order
   - Title bar with close button (red X)

4. **Focus System** (`gui/wm_core.c:wm_raise`, `gui/focus_manager.c`)
   - Z-order management
   - Focus tracking

5. **Drag System** (`gui/wm_events.c`)
   - Title bar click starts drag
   - Mouse move updates position
   - Mouse up ends drag
   - Bounds checking

6. **Close Window** (`gui/wm_core.c:wm_close`)
   - Close button click removes window

7. **Event Loop** (`gui/event_loop.c:event_loop_step`)
   - Mouse down/move/up handling

8. **App Integration** (`desktop/app_registry.c:desktop_app_launch`)
   - Apps launch via wm_create

## 🎯 Tasks to Complete/Verify

### ✅ Task 1: Verify WM initialization
- [ ] `wm_init()` is called in `gui_daemon.c`
- [ ] Already done in `gui_layer6_init()`

### ✅ Task 2: Verify window creation from Start Menu
- [ ] Click app in start menu creates window
- [ ] Already done in `desktop_app_launch()`

### ✅ Task 3: Verify window drag is working
- [ ] Test mouse down on title bar starts drag
- [ ] Test mouse move updates position
- [ ] Test mouse up ends drag
- [ ] Already implemented in `wm_events.c`

### ✅ Task 4: Verify Z-order (windows stacking)
- [ ] Clicking window brings it to front
- [ ] Already done in `wm_handle_mouse_down` -> `wm_raise()`

### ✅ Task 5: Verify close button works
- [ ] Clicking X button closes window
- [ ] Already done in `wm_handle_mouse_down`

### ✅ Task 6: Test the complete flow
- [ ] Build the kernel
- [ ] Run in QEMU
- [ ] Click Start -> Click Terminal
- [ ] Window appears
- [ ] Drag window by title bar
- [ ] Click on window to focus
- [ ] Click X to close

## 📋 Build Test Command

```bash
make clean && make
```

## ✅ Success Criteria (Strict)

Stage 2 is successful when:

1. ✅ Can open a window from Start Menu
2. ✅ Window can be dragged with mouse
3. ✅ Focus changes when clicking different windows
4. ✅ Windows stack properly (Z-order working)
5. ✅ X button closes the window

## 🔧 Files Modified

No files need modification - all features appear to be implemented.

However, need to verify:
- Taskbar click handling in `taskbar.c` (currently returns window id=1 always)
- Desktop click handling doesn't interfere with windows
