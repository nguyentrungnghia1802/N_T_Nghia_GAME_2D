# SDL2 Game Refactoring Task Plan

## Main Objective

Optimize the SDL2 C++ game so it runs smoothly on both powerful and weak
machines.

Primary goals:

-   Stable FPS.
-   Low CPU usage.
-   Low RAM usage.
-   No memory/resource leaks.
-   No frame-dependent gameplay.
-   Fast startup.
-   Maintain current gameplay behavior.

## Rules

-   Execute tasks sequentially.
-   One task at a time.
-   Build and verify after every task.
-   Do not rewrite the engine.
-   Do not introduce ECS/frameworks unless explicitly required.
-   Do not change gameplay intentionally.

# Step 1 --- Performance Baseline

Measure:

-   FPS.
-   Frame time.
-   CPU usage.
-   RAM usage.
-   Entity updates.
-   Rendering count.
-   Collision checks.
-   Runtime asset loading.

Create a before/after comparison.

# Step 2 --- Resource Safety

Status: **Completed on 2026-08-24** (`refactor(step-2)`).

Fix:

-   Memory leaks.
-   SDL resource cleanup.
-   Raw ownership problems.
-   Texture lifetime.
-   Font/audio cleanup.

Goal:

No RAM growth after restart or long sessions.

# Step 3 --- Remove Runtime Asset Loading

Never load textures during gameplay.

Replace:

-   IMG_Load during rendering.
-   SDL texture recreation.
-   Repeated font generation.

with:

-   preload.
-   cache.
-   reuse.

# Step 4 --- Fix Unsafe Code

Fix:

-   Undefined behavior.
-   Uninitialized values.
-   Broken include guards.
-   Invalid pointer usage.
-   Compiler warnings.

# Step 5 --- Improve Game Loop

Implement:

-   proper frame timing.
-   delta time.
-   stable update system.

Preserve current gameplay feeling.

# Step 6 --- Reduce CPU Usage

Implement:

-   active entity processing.
-   inactive entity sleeping.
-   rendering culling.
-   reduced unnecessary collision.

# Step 7 --- Optimize Collision

Improve:

-   collision queries.
-   hitbox handling.
-   active collision lists.

Do not change difficulty.

# Step 8 --- Reduce Runtime Allocation

Remove:

-   unnecessary new/delete.
-   per-frame allocations.
-   repeated texture creation.

# Step 9 --- Optimize Map System

Improve:

-   map ownership.
-   restart loading.
-   tile handling.

# Step 10 --- Portability

Prepare:

-   remove Windows-only runtime dependencies.
-   SDL-compatible architecture.
-   future Emscripten support.

# Final Acceptance

The game must:

-   run smoothly on weak hardware.
-   maintain stable FPS.
-   use reasonable CPU/RAM.
-   shutdown cleanly.
-   preserve gameplay.
