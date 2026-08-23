# Performance Audit

## Resolved in Step 3

### Modal text rendering churn

Game-over, win, time-limit, and menu labels now use retained `TextObject` caches. Static variants are prepared during startup; dynamic score/day values rebuild only when entering a state with changed content. The immediate `renderText` allocation path was removed.

## Remaining confirmed problems

### Uncapped menu rendering

Resolved in Step 6. Menu and static modal loops use the shared 60 Hz frame cap. A five-second settled-menu sample used 0.176% of the whole 16-logical-processor machine (2.81% single-core equivalent), down from the Step 1 7.23% whole-machine sample.

An event-driven redraw model could reduce this further, but is not required at the observed load.

### Full map copies every gameplay frame

`Map` contains 10,110 integers (about 40 KB before `std::string`/padding). `GameMap::getMap` returns it by value and `SetMap` copies it back each frame. This performs roughly 80 KB of explicit map copying per frame, around 4.8 MB/s at 60 Hz.

This bandwidth is probably small on a desktop, but it is confirmed unnecessary work and obscures ownership.

**Remedy:** Let the orchestrator obtain a mutable/reference view or move map update/draw behind `GameMap` methods.

### Frame-rate-dependent simulation

All movement, gravity, camera, respawn, and animation rates are per frame; calculated `delta_time` is unused. Slow frames slow gameplay, and faster frame delivery speeds it up. This is a performance/correctness coupling rather than merely a micro-optimization.

**Remedy:** Choose a fixed update step or consistently scale simulation with clamped delta time, calibrated to preserve current 60-ish-Hz feel.

### Per-frame active-target allocation (resolved)

Step 8 retains one `std::vector<ThreatCollisionTarget>`, reserves for the initial threat population, and clears/refills it without releasing capacity each frame.

**Result:** The recurring active-target heap allocation/free is removed. Expected benefit remains modest with 48 enemies.

## Potential problems

### Bullet/enemy collision complexity

Collision is O(active bullets x active threats), followed by O(total threats) `find_if` on a hit and vector erasure. Active-range filtering makes the current 48-enemy workload small. It becomes relevant only if bullet/enemy counts grow.

**Classification:** Potential; measure before adding a grid.

### Raw bullet allocation per shot (resolved)

Step 8 retains removed/inactive bullets in a `unique_ptr` pool and resets/reuses them on later shots. Heap allocation occurs only when simultaneous active bullets exceed the previous pool high-water mark; shutdown releases both containers.

**Result:** Focused tests confirm pointer reuse after hit-style removal and restart reset.

### Renderer state and overdraw

The background fills the screen, then player is drawn, then map tiles, then threats/Monster/HUD/text. Player-before-map causes overdraw and possible occlusion. No blend/state batching analysis was instrumented.

**Classification:** Potential visual/overdraw issue; probably minor at 1422x800.

### Profiler process queries

Every five seconds on Windows, the profiler snapshots all system threads to count those owned by this process. This is infrequent and outside most frames.

**Classification:** Potential small spike; measure if profiler remains enabled in release builds.

## Probably negligible at current scale

- Bounds-checked `vector::at` calls.
- `std::find_if` over at most 48 threats on a hit.
- Drawing at most about 19x10 visible tile positions, skipping blank IDs.
- Four enemy groups of 12, with only near-camera enemies updated.
- Text cache-key string/color comparisons.
- Two repeating background draw calls.
- `rand()` during threat creation/restart, not every frame.

## Existing performance strengths

- Character, bullet, and enemy textures are loaded once and borrowed.
- Temporary image surfaces are freed immediately after texture creation.
- HUD value guards prevent unchanged strings from being rebuilt, and `TextObject` prevents unchanged values from recreating textures.
- Bullet objects are pooled after removal/inactivity/restart and released at shutdown.
- The active collision scratch vector retains capacity across frames.
- Map drawing computes a visible tile range.
- Enemies outside the active camera margin are not updated/rendered/collided.
- Enemy render applies viewport culling.
- Bullet list is returned by const reference rather than copied.
- Restart restores cached map memory rather than re-reading/reloading tiles.

## Measurement limitations

The profiler measures update/render time before the frame-cap sleep. Step 1 captured menu-idle and gameplay-to-game-over stress data in `../performance-baseline.md`; it did not produce a full-level or low-end-hardware result. Performance severity beyond those exact scenarios is therefore **not confirmed from the current codebase**. Do not claim FPS/RAM improvements without rerunning a comparable scenario.
