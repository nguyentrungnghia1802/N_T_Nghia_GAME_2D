# Step 4 Unsafe-Code Report

Date: 2026-08-24

## Completed changes

- Corrected the `GameMap`, `ThreatObject`, and `PlayHealth` include guards and removed the `CommonFunc`/`TextObject` include cycle.
- Initialized projectile direction, player facing, enemy map/input state, text alpha, and text rectangles.
- Corrected the boolean enemy tile predicate, guarded flying-enemy tile indexes, and removed divide-by-zero-prone frame rectangle calculations.
- Added null guards to texture/text rendering and bounds guards to health-vector operations.
- Fixed text clip height and alpha handling.
- Made required startup loaders return/aggregate failure and stop before dereferencing missing assets.
- Validated exact map dimensions and tile IDs 0-7 before publishing map state; drawing also rejects unsupported IDs.
- Removed all warnings produced by `-Wall -Wextra -Wpedantic`.

## Verification

- Warning-clean full C++17 build with `-Wall -Wextra -Wpedantic`: passed.
- Double-include compilation for the three repaired headers: passed.
- Focused executable assertions: deterministic default bullet movement, first-click projectile creation, empty health decrease/increase safety, valid runtime map acceptance, and malformed text-file rejection: passed.
- Missing `res/pic/map/map01.txt` startup: exited cleanly with code `-1`; the original map was restored immediately.
- Automated menu-to-gameplay run and graceful shutdown: passed with exit code 0 and a profiler interval.

Collision geometry is intentionally deferred to Step 7. Step 4 corrected invalid frame dimensions, but the current shared collision helper still uses its historical hard-coded hitbox sizes, so gameplay difficulty is unchanged by that correction.
