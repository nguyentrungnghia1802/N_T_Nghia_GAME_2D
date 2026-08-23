# Performance Audit

## Confirmed problems

### Modal text rendering churn

`main.cpp::renderText` renders a new TTF surface, uploads a new texture, draws, and destroys both on every call. Game-over invokes it three times per modal-loop iteration; win invokes it three times. Those loops repeat until input with only a 1 ms delay. This is confirmed repeated CPU work, allocation, and texture upload during otherwise static screens.

**Remedy:** Build static screen text textures once on state entry, update only dynamic values, and render cached textures each frame.

### Uncapped menu rendering

`Call_Menu` continuously copies/presents the same screen with no delay and the renderer is not vsynced. On drivers where `SDL_RenderPresent` does not block, the menu can consume a full CPU core.

**Remedy:** Use the same central loop/frame pacing for all states, or wait for events/redraw only when hover state changes.

### Full map copies every gameplay frame

`Map` contains 10,110 integers (about 40 KB before `std::string`/padding). `GameMap::getMap` returns it by value and `SetMap` copies it back each frame. This performs roughly 80 KB of explicit map copying per frame, around 4.8 MB/s at 60 Hz.

This bandwidth is probably small on a desktop, but it is confirmed unnecessary work and obscures ownership.

**Remedy:** Let the orchestrator obtain a mutable/reference view or move map update/draw behind `GameMap` methods.

### Frame-rate-dependent simulation

All movement, gravity, camera, respawn, and animation rates are per frame; calculated `delta_time` is unused. Slow frames slow gameplay, and faster frame delivery speeds it up. This is a performance/correctness coupling rather than merely a micro-optimization.

**Remedy:** Choose a fixed update step or consistently scale simulation with clamped delta time, calibrated to preserve current 60-ish-Hz feel.

### Per-frame active-target allocation

The gameplay loop constructs a new `std::vector<ThreatCollisionTarget>`, calls `reserve`, fills it, and destroys it every frame. `reserve` still allocates because the vector is new each iteration.

**Remedy:** Reuse a retained scratch vector and clear it, or consume a filtered view. Expected benefit is modest with 48 enemies.

## Potential problems

### Bullet/enemy collision complexity

Collision is O(active bullets x active threats), followed by O(total threats) `find_if` on a hit and vector erasure. Active-range filtering makes the current 48-enemy workload small. It becomes relevant only if bullet/enemy counts grow.

**Classification:** Potential; measure before adding a grid.

### Raw bullet allocation per shot

Every click performs `new`, and each removal performs `delete`. At normal input rates this is unlikely to dominate. Rapid-fire features could make it noisy.

**Classification:** Potential; first change to `unique_ptr` for safety, not a pool for speed.

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
- `TextObject` prevents unchanged HUD strings from recreating textures.
- Map drawing computes a visible tile range.
- Enemies outside the active camera margin are not updated/rendered/collided.
- Enemy render applies viewport culling.
- Bullet list is returned by const reference rather than copied.
- Restart restores cached map memory rather than re-reading/reloading tiles.

## Measurement limitations

The profiler measures update/render time before the frame-cap sleep. No captured five-second gameplay log was available in this audit because the interactive game was not played through. Performance severity beyond the confirmed code paths above is therefore **not confirmed from the current codebase**. Do not claim FPS/RAM improvements without collecting comparable scenarios.

