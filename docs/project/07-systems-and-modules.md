# Systems and Modules

## `main.cpp`: application orchestration

**Purpose:** Entry point, SDL lifecycle, resource loading, menu/gameplay/modal screen control, enemy factory, collision orchestration, HUD timing, and shutdown.

**Main data:** All top-level SDL handles; cached textures; map/player/HUD objects; threat list; score/death flags; screen flags; `GameState`; timing values.

**Important functions:** `main`, `InitData`, `LoadFromFile`, `LoadRuntimeTextures`, `Call_Menu`, `MakeThreats`, `Restart`, `render_journey_img`, `Win_Game`, `close`.

**Dependencies:** Every application module and all SDL extension libraries.

**Lifecycle:** Globals are constructed before `SDL_main`; resources are filled after SDL initialization; `close` performs explicit cleanup; remaining global destructors run after `main` returns.

**Known concerns:** Monolithic responsibilities, broad global state, modal loops, unchecked load failures, unused state variables, implicit ownership, and tile-texture destruction order.

## `CommonFunc.*`: shared platform/game definitions

**Purpose:** SDL includes, constants, `Input` and `Map` structs, shared globals, rectangle collision, and menu hit-testing.

**State owned:** Definitions for global window/renderer/event, audio chunks used by player code, progression flags, background offset, camera checkpoint value.

**Callers:** Almost every module includes the header. `main.cpp` calls collision/hit-test; player and enemies read/write shared globals.

**Side effects:** Collision increments profiler counters; globals allow cross-module mutation.

**Known concerns:** God-header coupling, `CommonFunc.h` <-> `TextObject.h` cycle, hard-coded/asymmetric collision sizes, and compile-time map/screen constants.

## `BaseObject.*`: texture wrapper

**Purpose:** Load an image into an SDL texture, hold a render rectangle, render normally/repeating, free an owned texture, or borrow a cached texture.

**Owned data:** `SDL_Texture* p_object_`, `SDL_Rect rect_`, `bool owns_texture_`.

**Dependencies:** SDL_image/renderer APIs and profiler counters.

**Lifecycle:** `LoadImg` frees the prior texture and owns the new one; `UseTexture` frees prior state and borrows; destructor calls `Free`.

**Known concerns:** Default copy/move operations permit shallow copying of ownership state; render calls do not validate the texture/renderer; `Render1` mutates `rect_.x`; ownership is a runtime flag rather than a type.

## `GameMap` / `TileMat`

**Purpose:** Parse the 10x1011 integer map, retain base/runtime copies, advance/reset camera, calculate visible tile range, own tile textures, and render visible nonblank tiles.

**Owned data:** `Map game_map_`, `Map base_map_`, seven nonblank `TileMat` textures in an eight-ID array, base-map flag.

**Dependencies:** `Map`/constants/globals from `CommonFunc`, `BaseObject`.

**Lifecycle:** Map and seven nonblank tile textures load once; gameplay mutates the runtime map through a stable reference; the cached base map restores tiles/camera on replay without disk I/O.

**Known concerns:** Parsing/tile IDs, tight visible ranges, restart restoration, and shutdown order are validated. Player/enemy tile-sampling logic remains duplicated.

## `MainObject`: player and bullet owner

**Purpose:** Keyboard/mouse input, player texture selection/animation, gravity/movement/jump, tile collision and heart mutation, death/win detection, bullet creation/update/render/removal.

**Owned data:** Player kinematic/input/animation state, borrowed texture references, raw `std::vector<BulletObject*>`, heart count.

**Dependencies:** Map data, global winner flag, audio chunks, `BaseObject`, `BulletObject`, profiler.

**Lifecycle:** Global object; cache references assigned after load; bullets are uniquely owned and released by vector erase/clear/destruction.

**Known concerns:** Uninitialized initial fire direction, erase-while-indexing skips a bullet, raw-pointer ownership API, frame-dependent physics, incorrect frame rectangle width, mixed player/bullet/score responsibilities, and several misspelled public names.

## `BulletObject`

**Purpose:** Store direction/speed/active flag and move horizontally until leaving the screen.

**Owned data:** Movement values, active flag, direction; borrowed texture through `BaseObject`.

**Caller:** Only `MainObject` creates/updates/owns bullets; `main.cpp` reads the list for enemy collision.

**Known concerns:** `bullet_dir_` is uninitialized, vertical value/border are unused, and ownership is raw/manual.

## `ThreatsObject`

**Purpose:** Enemy frame-strip setup, borrowed texture switching, active enemy physics, map collision, simple patrol direction, rendering, and respawn logic.

**Owned data:** World velocity/position, frame clips, movement type/patrol bounds, input flags, borrowed texture references.

**Caller:** `main.cpp` creates, activates, updates, renders, collides, and erases enemies.

**Known concerns:** Global `change_threats` coupling and duplicated tile-response logic remain. Step 4 fixed initialization, guards, indexes, predicate, and frame dimensions.

## `PlayerPower` and `PlayerMoney`

**Purpose:** Render health icons and the heart icon from startup texture caches.

**Owned data:** Each borrows a cached texture via `BaseObject`; health owns a list of X positions.

**Lifecycle:** Cache references are assigned at startup; health positions are reset on replay without image I/O; borrowers are cleared before cache owners in `close`.

**Known concerns:** Unused public mutators remain; vector edge cases are guarded.

## `TextObject`

**Purpose:** Convert text to an owned SDL texture, cache by text/font/RGB, and render it.

**Owned data:** String/color cache, owned texture, dimensions; borrowed font pointer.

**Lifecycle:** Rebuilds only when cache key changes; destructor/`close` frees texture before fonts are closed.

**Known concerns:** Texture ownership remains manual. Step 4 removed the include cycle and validates font/renderer/texture state, alpha, and clip dimensions.

## `Profiler`

**Purpose:** Count entity updates/renders, collisions, loads, texture/text creation; time update/render work; log five-second summaries; query Windows process CPU/RAM/thread count. `GAME_PROFILE_LOG` optionally mirrors each line to a file for reproducible capture.

**State:** Internal anonymous-namespace singleton-style counters.

**Lifecycle:** Initialized after SDL; interval starts after gameplay setup.

**Known concerns:** Global mutable counters, Windows-only process metrics (zero elsewhere), `frame_ms` excludes cap delay, and no runtime switch to disable output. CPU is normalized across logical processors and must be interpreted with the machine configuration.

## `ImpTimer`

**Purpose:** Legacy start/stop/pause tick timer.

**Current use:** No application call sites found; it is still compiled and linked.

**Known concerns:** Strong dead-code candidate. Remove only after confirming no external consumer depends on its headers/object.

## Map conversion utility

`res/pic/map/TILE_MAP/Build Map/main.cpp` is a separate tiny console tool that converts commas in `input.txt` to spaces in `map.txt`. It is not part of the game Makefile. Its output differs from the current runtime `map01.txt`, so the promotion step into the game is **not confirmed from the current codebase**.
