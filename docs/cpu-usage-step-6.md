# Step 6 CPU-Usage Report

Date: 2026-08-24

## Completed changes

- Applied the shared 60 Hz cap to menu, game-over, win, and journey loops.
- Reused the player collision rectangle once per gameplay frame instead of rebuilding it for every active enemy.
- Revalidated existing camera-range sleeping, viewport culling, bullet culling, and active-target collision filtering.

## Verification

- Warning-clean C++17 build with `-Wall -Wextra -Wpedantic`: passed.
- Five-second settled-menu sample on the same 16-logical-processor machine: 0.176% whole-machine CPU, 2.81% single-core equivalent, 120.3 MiB working set.
- Step 1 settled-menu baseline: 7.23% whole-machine CPU and approximately 115.66% single-core equivalent.
- Menu startup and graceful close: passed with exit code 0.

The before/after CPU samples were taken on the same machine but on different runs; they demonstrate removal of the busy loop, not a universal hardware benchmark.
