# Step 11 Respawn Input Report

Completed on 2026-08-24.

## Root cause

- Direction flags were event-latched. The one-second death wait pumps and consumes events without forwarding key-up events to `MainObject`, so a pre-death `A`/`D` state could remain active.
- Comeback completion unconditionally added 100 pixels to `x_pos_`, producing a visible forward jump even with no input.

## Fix

- Added one `PrepareRespawn` entry point that clears directional/jump input, horizontal velocity, and movement animation before setting the comeback timer.
- Routed collision death, map/fall death, replay, and win restart through that entry point.
- Removed the unconditional 100-pixel normal-respawn offset.
- Preserved the existing camera catch-up only when the player is already behind the auto-scrolling viewport, where retaining the invalid X position would immediately trigger another death.

## Verification

- `tests/respawn_input_tests.cpp`: passed; a latched `D` key does not survive respawn and X remains exactly unchanged across comeback completion and the following simulation frame.
- Full GNU Make rebuild with `-Wall -Wextra -Wpedantic`: passed with no diagnostics.
- Runtime startup and SDL quit-event smoke test: window created and exited with code 0.

The focused test covers the reported movement regression. A complete manual death/replay visual playthrough remains separate from this automated check.
