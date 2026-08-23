# Step 7 Collision Report

Completed on 2026-08-24.

## Changes

- Replaced the asymmetric corner-inside query with a dimension-aware AABB query.
- Preserved the original non-collision rule for edge-only contact.
- Made the historical 115x95 player/bullet and 150x100 threat footprints explicit, preserving gameplay difficulty.
- Precomputed threat hitboxes for the active collision list and stopped appending enemies already consumed by player collision.
- Added focused collision regression tests.

## Verification

- Full build with `-Wall -Wextra -Wpedantic`: passed with no diagnostics.
- `tests/collision_tests.cpp`: passed edge-touch, containment, four-direction overlap, invalid-size, and footprint assertions.
- Runtime startup and SDL quit-event smoke test: SDL window created and exited with code 0.

The automated smoke test does not claim a full manual playthrough or level completion.
