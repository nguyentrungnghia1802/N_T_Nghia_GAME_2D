# Step 8 Runtime Allocation Report

Completed on 2026-08-24.

## Changes

- Retained the active collision-target scratch vector and its capacity across frames.
- Added a `unique_ptr` bullet pool; inactive, hit, and restart bullets are reset and reused instead of destroyed/reallocated.
- Kept full bullet-pool release in application shutdown.
- Guarded HUD string construction by the displayed time, heart, and high-score values.
- Replaced explicit threat `new` expressions with `make_unique` at non-frame factory boundaries.
- Continued using the startup texture/text caches established in Step 3; no repeated gameplay image loads were introduced.

## Verification

- Full build with `-Wall -Wextra -Wpedantic`: passed with no diagnostics.
- `tests/runtime_allocation_tests.cpp`: confirmed bullet creation, hit-style recycling, restart recycling, pointer reuse, and shutdown clearing.
- Runtime startup and SDL quit-event smoke test: window created and exited with code 0.

The smoke test does not claim a complete manual playthrough or a platform-wide allocator benchmark.
