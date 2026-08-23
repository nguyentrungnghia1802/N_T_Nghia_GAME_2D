# Code-Quality Audit

## Correctness and robustness findings

### Broken include guards

Three headers test one macro and define a different macro:

- `src/gamemap.h:2-3`: `GMAE_MAP_H` versus `GAME_MAP_H`.
- `src/ThreatObject.h:1-2`: `THREAT_OBJECT_H_` versus `THREAT_OBJECT_h_`.
- `src/PlayHealth.h:2-3`: `PLAYER_POWER_H_` versus `PLAY_POWER_H_`.

Double-including each header was compiler-tested and produced class redefinition errors. Current translation units happen not to trigger the duplicate path, but include order is fragile.

### Journey transitions are unreachable in normal scrolling

`GameMap::MapRun` adds six to a camera starting at zero (`src/gamemap.h:38`). `render_journey_img` requires exact equality with `280 + n*16170` (`src/main.cpp:1077-1084`). Every target is 4 modulo 6, so normal scrolling never reaches one. This also prevents the journey-controlled `change_threats` toggles from occurring normally.

### Initial bullet direction is indeterminate

`BulletObject::bullet_dir_` is not initialized (`src/BulletObject.cpp::BulletObject`). Player facing starts at `-1` (`src/MainObject.cpp:24`). A mouse click before an `A`/`D` event enters neither direction branch (`src/MainObject.cpp:252-261`), then `HandleMove` reads the uninitialized enum-like value. This is undefined behavior and can leave an active bullet that never exits.

### Collision implementation (resolved)

Resolved in Step 7. `SDLCommonFunc::CheckCollision` now performs a symmetric AABB query with supplied dimensions and focused regression coverage. Call sites explicitly preserve the historical 115x95 player/bullet and 150x100 threat footprints, so the correctness fix does not silently alter game difficulty.

### Chained enemy collision comparison

`src/ThreatObject.cpp:258` evaluates `val2 != BLANK_TILE != POINT_ITEM_1`, a left-associative comparison of a boolean with `1`, rather than checking that tile 2 is neither blank nor a heart. `-Wall` explicitly warns on this expression. Upward enemy/tile collision can therefore make the wrong decision.

### Asset failure propagation (resolved)

Resolved in Step 4. Startup loaders return success, required handles are aggregated, screen conversion only runs after surface validation, and map/tile failures stop before the menu.

### Erasing inactive bullets skips the next element (resolved)

Resolved. `MainObject::HanleBullet` retains the current index after recycling an inactive bullet, so a shifted element is processed immediately. Step 8 also reuses the bullet allocation on later shots.

## Duplication

| Duplication | Evidence | Impact |
| --- | --- | --- |
| Tile collision | Player and enemy each implement similar horizontal/vertical tile sampling | Bugs such as the chained comparison survive in one copy |
| Clip construction | Player assigns 6 rectangles manually; enemy assigns 5 manually | Verbose and error-prone if frame count changes |
| Modal screen loops | Menu, journey, game-over, win each own render/event/quit loops | Inconsistent input/timing and difficult transition changes |
| Raw resource cleanup | Repeated `FreeSurface`, `DestroyTexture`, `CloseFont`, `FreeChunk` calls and many globals | Easy to miss a new handle |
| Immediate TTF rendering | `TextObject` and `main.cpp::renderText` implement parallel surface/texture paths | Modal path bypasses cache and repeats work |
| Threat setup | Four near-identical loops in `MakeThreats` | Spawn and texture rules are scattered through constants |

## Coupling and cohesion

`src/main.cpp` has 1219 lines and at least these unrelated responsibilities: platform init, asset repository, scene controller, game loop, camera, collision coordinator, HUD, enemy factory, audio playback, timing, and teardown. It is the dominant maintainability bottleneck.

`CommonFunc.h` is a god header. It exposes SDL extension headers, core types/constants, audio/window/renderer globals, and progression globals to almost every module. It unnecessarily includes `TextObject.h`, while `TextObject.h` includes it back.

`MainObject` combines player movement, tile collision, score collection, sound side effects, bullet allocation/ownership/update/render, death state, and win detection. `ThreatsObject` depends on the global `change_threats` rather than receiving an explicit appearance/region state.

## Significant global state

| Group | Examples | Impact |
| --- | --- | --- |
| SDL platform | `g_window`, `g_screen`, three event globals | Hidden lifetime and broad reach |
| Resource repository | 11 global `BaseObject`s, 6 fonts, surfaces/textures, 8 audio pointers | Initialization order and cleanup are manual |
| World | `game_map`, local runtime-map reference, `p_player`, `threats_list` | Map authority is explicit; no top-level application owner |
| HUD/progress | `player_power`, `player_heart`, text objects, scores/deaths | State transitions mutate distant globals |
| Control | `winner`, `is_quit`, `start_Game`, restart flags, `change_threats`, `game_state` | No single authoritative screen state |
| Timing/render | `start_time`, `current_time`, `time_render`, `minus`, `delta_time` | Some values have no behavioral consumer |

## Magic values and configuration debt

- Screen size, frame cap, tile/map dimensions, journey length, speeds, gravity, jump, enemy count, and active margin are compile-time constants across multiple headers.
- Enemy spawn formulas repeat values such as 500, 780, 100, and random ranges (`src/main.cpp::MakeThreats`).
- Player win detection uses `MAX_MAP_X*TILE_SIZE - 16*TILE_SIZE`; the unused `POS_MAP_DATA_START_X_TO_WIN` has a different value.
- Restart checkpoints use hard-coded `+280`, `+500`, and only three camera ranges (`src/main.cpp::Restart`, `src/gamemap.cpp::ResetMap`).
- Collision intentionally uses named 115x95 player/bullet and 150x100 threat hitboxes to preserve established gameplay.
- Start/replay and death waits use 4000/1000 ms inline.

These constants encode current gameplay. Name/consolidate them before changing values; do not “clean them up” in a way that silently changes feel.

## Dead or unused code/state

Strong call-site evidence identifies:

- Entire `ImpTimer` module: compiled, no runtime call sites.
- `PlayerPower::SetNum` and `InitCrease`: no call sites.
- `MainObject::set_bullet_list`: no call sites and unsafe ownership semantics.
- `gFont1`, `gFont2`: loaded/freed, never used to render.
- `gMusic`: never loaded or played.
- `focus_mouse`: defined, never read/written elsewhere.
- `delta_time`: calculated/stored, never consumed.
- `game_state`: assigned in transitions, never read.
- `SCREEN_BPP`, `POS_MAP_DATA_START_X_TO_WIN`: no behavioral use.
- Bullet Y value and Y border: stored/passed but not used in movement.
- `val3`/`val4` in flying-enemy collision: read, never used; compiler warns.

Unused assets are not automatically dead code: many are README images, Tiled sources, or art working files.

## Naming and interface issues

Examples include `HandelInputAction`, `HanleBullet`, `RetsetMinusLive`, `is_minusLinve`, `GMAE_MAP_H`, `ThreatsObject` for one enemy, “money” for hearts, and parameter names `boder`. `CommonFunc` contains both constants and state. `LoadFromFile` does not say which asset group or report success. `close` collides conceptually with common APIs and hides that it shuts down the whole application.

Naming cleanup should follow behavior tests so it can be mechanical and low-risk.

## Inconsistent patterns and compiler warnings

- `NULL` and `nullptr` are mixed.
- Macros and `const int` are mixed for constants.
- `int` indices are compared with `size_t` container sizes.
- `MainObject::set_clips` uses bitwise `&` between boolean expressions.
- `SDL_Color` alpha fields are omitted.
- Several function parameters and local variables are unused.
- Include guards are inconsistent and broken in three headers.
- Runtime source uses `unique_ptr` for threats and active/pooled bullets; SDL resources retain explicit owner/borrower handling.

## Documentation/build drift

The user READMEs and `#rungame.txt` omit `Profiler.cpp`. `docs/refact.md` still describes pre-refactor leaks/reloads that HEAD has addressed. `main.exe` and object files are tracked, and there is no `.gitignore`; a prebuilt binary therefore cannot be assumed to represent current source.
