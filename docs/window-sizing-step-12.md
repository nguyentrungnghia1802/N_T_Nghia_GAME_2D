# Step 12 Window Sizing Report

Completed on 2026-08-24.

## Root cause

`InitData` always requested a physical 1422x800 SDL window. On a 12-inch-class display such as 1366x768, the client area plus window decorations could not fit in the usable desktop area.

Screen inches are not a reliable runtime input, so the fix uses the display's usable pixel bounds. This covers different laptop sizes and DPI configurations without hard-coding a model name.

## Fix

- Added `WindowConfig::CalculateWindowSize`, a pure helper that preserves the 1422:800 aspect ratio and reserves space for the taskbar/window decorations.
- `InitData` centers the calculated size in display 0 and creates only `SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE`; no fullscreen flag is set.
- `SDL_RenderSetLogicalSize` keeps the existing 1422x800 gameplay coordinate system while SDL scales rendering into the fitted window.
- Menu mouse events are converted with `SDL_RenderWindowToLogical`, so Start/Exit hit testing remains correct after scaling or resizing.

## Verification

- `tests/window_config_tests.cpp`: passed for 1920x1080, 1366x768, 1024x600, aspect-ratio preservation, and invalid-bound fallback.
- Full GNU Make rebuild with `-Wall -Wextra -Wpedantic`: passed with no diagnostics.
- Runtime SDL smoke test: created a 1438x839 window on a 1920x1080 work area, confirmed it stayed inside the work area and was not maximized, then closed with exit code 0.

The exact physical size in inches cannot be asserted from SDL; sizing is intentionally resolution/usable-area based.
