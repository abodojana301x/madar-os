## GUI Quality Improvement Plan

### Problem Summary
- The system looks pixelated because the graphics stack is still very primitive.
- Visual quality is limited by low-level raster drawing, tiny bitmap fonts, and the current QEMU + VNC display path.
- Styling alone cannot fix the result unless the rendering foundation is improved first.

### Root Causes
- Fixed internal rendering resolution is too low for a modern desktop feel.
- Text rendering uses a very small bitmap font, which makes labels and UI text look broken and rough.
- Drawing primitives are rendered with hard edges and no anti-aliasing.
- Transparency and blur are not implemented as true composited effects.
- The display path goes through QEMU, VNC, noVNC, and the browser, which amplifies scaling artifacts.

### Execution Plan
1. Raise the internal rendering resolution.
   - Move from the current fixed low-resolution path to a higher desktop-friendly mode such as 1600x900 or 1920x1080.
   - Keep framebuffer mode and GUI surface size aligned as much as possible.

2. Replace the current tiny bitmap font.
   - Introduce a larger and cleaner bitmap font first as a practical step.
   - Then move to a more capable font rendering path if the engine allows it.

3. Improve primitive rendering quality.
   - Add smoother rounded rectangles, circles, borders, and lines.
   - Reduce visible stair-stepping on window corners, icons, and buttons.

4. Implement real alpha blending.
   - Replace the current approximate blending behavior with proper per-pixel color compositing.
   - Use this as the base for glass, highlights, shadows, and overlays.

5. Add glass and blur effects correctly.
   - Implement frosted panels and window surfaces using proper sampled blur behind translucent layers.
   - Apply this to taskbar, start menu, and window chrome.

6. Improve the display path.
   - Reduce scaling mismatch between guest framebuffer and browser viewport.
   - Prefer display settings that avoid unnecessary browser-side magnification.

7. Redesign after the rendering foundation is fixed.
   - Only after resolution, fonts, and blending improve should the Windows 11 style pass continue.
   - Then refine taskbar, start menu, icons, and window composition.

### Recommended Order
1. Resolution
2. Fonts
3. Anti-aliased primitives
4. Real alpha blending
5. Blur and glass
6. Visual redesign pass

### Expected Outcome
- Cleaner text
- Less visible pixelation
- Softer edges
- More convincing Windows 11 style
- Glass effects that look intentional instead of fake
