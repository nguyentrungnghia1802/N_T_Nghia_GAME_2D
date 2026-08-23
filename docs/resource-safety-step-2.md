# Step 2 Resource Safety Report

Date: 2026-08-24

## Completed changes

- Deleted `BaseObject` copy operations so SDL texture ownership cannot be shallow-copied.
- Added explicit, idempotent `GameMap::FreeTiles` cleanup before renderer destruction.
- Reordered shutdown to stop audio, release entities and SDL resources, close the audio device, then destroy the renderer/window and quit SDL.
- Replaced the raw owning bullet vector with `unique_ptr` ownership and erase-safe iteration.
- Cleared live bullets on replay as well as shutdown.

## Verification

- Full C++17 build with the Makefile source list and standard warnings: passed.
- Strict build with `-Wall -Wextra -Wpedantic`: passed with the known warning backlog reserved for Step 4.
- Five repeated startup/menu-close cycles: all exited with code 0; sampled working sets were 121.20, 119.92, 120.11, 128.09, and 119.95 MiB, with no monotonic growth across independent runs.
- Active gameplay shutdown after startup audio and eight projectile inputs: exited with code 0.
- Runtime profiler interval during that gameplay test: 36.92 average FPS, 0.35% whole-machine CPU, and 147,824 KiB working set. This is a functional lifetime check, not a performance improvement claim.

No supported memory/error checker was installed in the current environment (`libasan` runtime and Dr. Memory were unavailable). The repeated replay path was verified by code inspection and by exercising the same `ClearBulletList` destruction path with live projectiles during shutdown; an automated full death/replay cycle is not currently available.

## Deferred in task order

- Missing/corrupt asset validation and existing compiler warnings belong to Step 4.
- Runtime asset caching belongs to Step 3.
- Pooling/reducing projectile allocations belongs to Step 8.
