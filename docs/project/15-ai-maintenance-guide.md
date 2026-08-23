# AI Maintenance Guide

## Before changing code

1. Check repository instructions in `docs/agent.md` and working-tree status.
2. Treat this knowledge base as the current architecture reference; treat `docs/refact.md` as historical unless revalidated against HEAD.
3. Read the full functions that call and are called by the target code. In this project, local changes often depend on global state in `main.cpp`/`CommonFunc.cpp`.
4. Use `Makefile` as the canonical source list. Older README commands omit `Profiler.cpp`.
5. Do not claim gameplay/runtime behavior passed without interactive execution. No automated test suite exists.

## Behavioral invariants to preserve unless the task explicitly changes them

- Fixed 1422x800 window and 80-pixel tiles.
- Runtime map is 1011 columns x 10 rows from `res/pic/map/map01.txt`.
- IDs: 0 blank, 1 collectible heart, other positive runtime IDs solid/rendered.
- Controls: `A` left, `D` right, `W` jump, left mouse fire.
- Four enemy groups of 12 with current region/spawn formulas.
- Three health icons; the fourth death enters game over.
- Hearts removed from runtime map on collection and restored from base map on replay.
- Threats and bullets borrow global cached textures; caches must outlive all borrowers.
- High score is process-local; no persistence contract exists.
- Renderer/window/SDL must outlive every texture/surface/font draw operation.

Known defects are not invariants. In particular, do not preserve unreachable journey equalities, indeterminate first-shot direction, chained enemy collision, or flawed hard-coded AABB simply because they are current behavior. Fix them in explicitly scoped correctness work with regression evidence.

## Resource ownership rules

```text
global texture cache owns SDL_Texture
    -> player / threat / bullet BaseObject borrows via UseTexture

ThreatList owns ThreatsObject via unique_ptr
MainObject owns BulletObject via BulletList unique_ptr entries
TextObject owns text texture and borrows TTF_Font
GameMap TileMat owns tile textures
main.cpp owns window, renderer, raw screen resources, fonts, and audio
```

When changing cleanup:

1. Stop playback/transitions that can use resources.
2. Destroy threats and bullets.
3. Clear borrower pointers/objects.
4. Destroy cached/tile/UI/text textures and surfaces.
5. Close fonts and audio chunks/device.
6. Destroy renderer, then window.
7. Quit SDL extensions and SDL.

The current `close` path follows this order, including explicit tile cleanup and `Mix_CloseAudio`. Preserve it when adding resources.

Never copy a `BaseObject` or derived object by value until copy operations are made safe. `UseTexture` means non-owning; `LoadImg` means owning.

## Tightly coupled and dangerous areas

| Area | Coupling/risk | Required context before edit |
| --- | --- | --- |
| `main.cpp` gameplay loop | Interleaves update, render, collision, modal states, HUD | `05-runtime-flow.md`, `06-game-loop.md` |
| Player tile collision | Mutates world position, map hearts, score, sound, win/death flags | `MainObject::DoPlayer` and full `CheckToMap` |
| Enemy tile collision | Movement type, patrol, cached texture, global appearance | Full `ThreatsObject::CheckToMap`/`ImpMoveType` |
| Collision helper | Used for both player/enemy and bullet/enemy | All call sites and intended hitboxes |
| Restart/checkpoints | Depends on `winner`, `map_start`, base map, player world X | `Restart`, `GameMap::ResetMap`, journey logic |
| Texture caches | Runtime owner/borrower ordering | `LoadRuntimeTextures`, `ConfigureDynamicThreat`, `close` |
| Modal screens | Own their own event loops and audio delays | Menu/game-over/win/journey functions plus outer loop |
| Map artifacts | Multiple different map text files exist | Only `res/pic/map/map01.txt` is runtime canonical |

## Safe feature workflow

### Adding an image/animation

Load it once during validated startup, document the owner, pass a borrowed pointer only to objects with shorter lifetimes, and free the owner before renderer shutdown but after borrowers. Do not call `IMG_Load` from update/render. For a frame strip, verify divisibility by frame count and generate/test clips.

### Adding an enemy variant

Keep `ThreatList` ownership, configure cached textures before spawning, preserve active-range filtering, and decide explicitly whether the enemy sleeps outside range. Update collision only through explicit hitbox data. Avoid another copied setup loop; use the small factory configuration proposed in the roadmap.

### Adding a UI label

Use `TextObject` or another cache whose texture updates only when text/font/color changes. Do not use `renderText` inside a repeating static screen loop. Ensure the font outlives the text texture.

### Adding a screen/state

Do not add another nested modal loop. Prefer first implementing/using the central state dispatcher. Specify accepted input, transition triggers, timers, audio enter/exit actions, and whether gameplay time pauses.

### Changing map/progression

Update Tiled source and deliberately export/promote the runtime map; the existing converter output is not automatically canonical. Validate exact dimensions and tile IDs. Revisit `MAX_MAP_X/Y`, journey distances, win threshold, spawn/checkpoint positions, and camera clamp together.

## Safe refactoring workflow

1. State the one behavior/ownership objective.
2. Capture current compiler warnings and a focused interactive scenario.
3. Make one ownership/control-flow change, not a broad rename at the same time.
4. Build with current Makefile flags plus `-Wextra -Wpedantic`.
5. Exercise the affected transition and shutdown path.
6. Check five-second profiler output for loads/textures during gameplay.
7. Update the canonical `docs/project` entry if architecture, ownership, or behavior changed.

For collision/timestep changes, create focused pure tests before restructuring. For resource changes, run repeated start/replay/exit cycles and a memory/error checker if available.

## What not to change casually

- Do not replace the whole design with ECS or a framework.
- Do not change physics constants while converting to delta/fixed time; first preserve 60 Hz behavior.
- Do not free/reload cache textures while any player/enemy/bullet borrows them.
- Do not make `Map` dimensions dynamic without auditing every fixed array/index/threshold.
- Do not alter render order without visual validation; current order has map-after-player and a Monster overlay.
- Do not remove unreferenced art merely because runtime search finds no call site.
- Do not trust tracked `main.exe`/`.o` as proof that current source builds.
- Do not report historical issues from `docs/refact.md` as current without checking HEAD.
- Do not combine correctness, architecture, timing, naming, and artifact cleanup in one patch.

## Minimum validation matrix

| Change area | Minimum scenarios |
| --- | --- |
| Startup/resources | Normal start; one missing required asset; window close |
| Input/bullets | Fire before movement; left/right fire; consecutive bullets leave screen |
| Collision | Edge touch, containment, left/right/top/bottom overlap; bullet and player cases |
| Map/hearts | Collect from horizontal/vertical approach; replay restores hearts |
| Progression | Cross all five boundaries once; death around each checkpoint; win/replay |
| Timing/state | Long menu wait, game-over wait, win wait; timer pauses/resets per defined rule |
| Ownership | Multiple restarts and exits; no load after gameplay starts; no double-free/leak reports |
| Performance | Comparable profiler interval before/after in the same map region |

If a result cannot be observed or inferred from code, state: **Not confirmed from the current codebase.**
