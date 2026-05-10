# Madar Desktop Environment (MDE) Evolution Plan - v2.0 (May 2026)

The MDE Evolution Plan aims to transform the current experimental shell into a production-ready, Arabic-first desktop environment. This updated plan addresses technical gaps identified in v0.1 and prioritizes the infrastructure needed for a modern user experience on Madar OS.

## 1. Foundation: The Arabic-First Core
The primary goal of Madar OS is to lead in Arabic computing. The current placeholder font engine must be replaced with a robust typography system.

| Objective | Description | Priority |
| :--- | :--- | :--- |
| **Arabic Shaping Engine** | Implementation of a lightweight HarfBuzz-like shaping logic for Arabic ligatures and glyph joining. | Critical |
| **Bidi Support** | Integration of the Unicode Bidirectional Algorithm (UBA) to handle mixed Arabic and English text. | High |
| **Vector Font Support** | Transition from placeholder boxes to TrueType/OpenType font rendering (e.g., using a port of stb_truetype). | High |
| **RTL Layout Logic** | A global layout system that automatically flips UI components (Taskbar, Menus, Dialogs) when the language is Arabic. | Critical |

## 2. Advanced Window Management (WM)
Moving beyond simple rectangles, the Window Manager needs to handle user interaction gracefully and support modern multitasking.

The Window Manager will be upgraded to support **Dynamic Geometry Storage**, allowing windows to remember their previous size and position before being maximized or minimized. Additionally, a **Keyboard Shortcut Daemon** will be introduced to handle global hotkeys like `Alt+Tab` for window switching and `Super+D` to show the desktop. Visual improvements will include **Window Decorations** (Title bars with Arabic text support and functional buttons) and **Soft Shadows** using simple alpha-blending to improve depth perception.

## 3. The Madar UI Framework (MUI)
To ensure consistency across applications, a centralized UI toolkit is required. This framework will move away from manual coordinate positioning.

> "A great desktop environment is defined by its consistency. Developers should not have to calculate pixel positions for a button."

*   **Flex-Layout Engine:** A simple box-model layout system (similar to Flexbox) to handle responsive UI resizing.
*   **Theming Engine:** External CSS-like configuration files to define colors, borders, and spacing without recompiling the OS.
*   **Event Delegation:** A centralized event system where widgets can subscribe to `onClick`, `onHover`, and `onFocus` events.

## 4. Native Application Suite
Transforming backend logic into functional graphical applications.

| App | 2026 Roadmap | Status |
| :--- | :--- | :--- |
| **Madar Terminal** | Full ANSI escape code support and Arabic RTL command input. | Backend Only |
| **Madar Files** | Icon-based grid view, breadcrumb navigation, and integrated file search. | Logic Ready |
| **Control Center** | A unified settings app for Display, Network, and Language localization. | Drafted |
| **Task Manager** | Real-time CPU/RAM graphs and process tree visualization. | Logic Ready |

## 5. Desktop Services & Integration
The "glue" that makes the desktop feel like a cohesive system.

A **Notification Daemon** will be implemented to provide non-intrusive system alerts in the top-right (or top-left for RTL) corner. The **Session Manager** will be enhanced to support "Suspend to Disk," saving the state of open windows and applications. Finally, a **Global Search (Madar Search)** will be integrated into the Start Menu to allow users to find files, apps, and settings instantly.

## 6. Implementation Strategy
1.  **Q2 2026:** Complete the Arabic Shaping Engine and integrate it into the `font_engine.c`.
2.  **Q3 2026:** Deploy the MUI Framework and port the current Start Menu and Panel to use it.
3.  **Q4 2026:** Release the first "Beta" of the Native Application Suite (Files, Terminal, Settings).
4.  **2027:** Open the `libmadar_ui` SDK for third-party developers.
