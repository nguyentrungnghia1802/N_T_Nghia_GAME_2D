# Technical-Debt Inventory

Severity uses P0 for correctness/crash/memory-safety work, P1 for major maintainability/timing issues, P2 for architecture/duplication, and P3 for optional cleanup.

| ID | Severity | Area | Problem | Impact | Suggested fix |
| --- | --- | --- | --- | --- | --- |
| TD-001 | P0 | Progression | Journey uses unreachable exact camera equalities | Journey panels and threat-style toggles do not occur in normal scrolling | Detect crossing/ranges and record one-shot journey progress |
| TD-002 | P0 | Input/bullets | Initial player facing and `bullet_dir_` leave first-shot direction indeterminate | Undefined behavior; stuck projectile | Initialize facing/direction and always assign a fire direction |
| TD-003 | P0 | Collision | Shared collision ignores real sizes and misses overlap cases | False hits/misses for player and bullets | Use `SDL_HasIntersection` or a tested AABB helper with explicit hitboxes |
| TD-004 | P0 | Enemy physics | Chained `val2 != BLANK_TILE != POINT_ITEM_1` | Incorrect upward tile collision | Correct predicate and add tile-collision cases |
| TD-005 | P0 | Startup | Asset/map failures are ignored then pointers/indices are used | Crash/undefined rendering on missing or corrupt data | Return structured load failure and validate before use |
| TD-006 | P0 | Headers | Three include guards define the wrong macro | Re-definition compile failures under different include order | Correct guards or use `#pragma once`; add double-include compile check |
| TD-007 | P0 | SDL lifetime | Tile textures outlive renderer shutdown; audio device not closed | Invalid texture destruction/resource leak risk | Destroy tiles before renderer; call `Mix_CloseAudio` |
| TD-008 | P0 | Ownership | `BaseObject` is shallow-copyable despite owning raw textures | Future double-free/use-after-free on copies | Delete copy operations; implement safe move or RAII handle |
| TD-009 | P1 | Timing | `delta_time` is unused; simulation is frame-based; cap targets 16 ms | Gameplay speed depends on frame rate | Introduce fixed step or consistent delta scaling |
| TD-010 | P1 | State flow | Menu/journey/game-over/win are nested modal loops | Duplicated input/timing; blocks portable main loop | Central state-driven loop with enter/update/render/exit |
| TD-011 | P1 | Architecture | `main.cpp` owns nearly everything through globals | High change risk and poor testability | Extract a small top-level `Game`/resource owner incrementally |
| TD-012 | P1 | Bullets | Raw owning pointer vector; erase loop skips shifted inactive bullet | Fragile lifetime and delayed cleanup | `vector<unique_ptr<BulletObject>>`; iterator/index-safe erase |
| TD-013 | P1 | Time/score | Initial timer includes menu; replay/reset semantics are inconsistent | Displayed completion time is misleading | Define play-time semantics and update only in Playing state |
| TD-014 | P1 | Map | Full 10x1011 map copied out and back every frame | Unnecessary work and unclear authority | Mutate through `GameMap` reference/API |
| TD-015 | P1 (resolved) | UI performance | Static modal text was rasterized/uploaded every iteration | Fixed in Step 3 with startup/state-entry text caches | Preserve cache-only modal rendering |
| TD-016 | P1 | Menu performance | Menu loop has no pacing/vsync | Possible high idle CPU | Event-driven redraw or central cap |
| TD-017 | P2 | Map safety | Parser does not verify 10,110 reads or tile range before indexing | Malformed data can access wrong tile texture | Validate count, stream state, and IDs 0-7/array bounds |
| TD-018 | P2 | Duplication | Player/enemy tile collision and map loaders duplicate logic | Divergent fixes and subtle bugs | Share small tile-query/predicate helpers; remove unused loader |
| TD-019 | P2 | Includes | `CommonFunc.h`/`TextObject.h` cycle and broad SDL/global exposure | Fragile includes and slow/broad recompilation | Split constants/types/globals; use direct includes/forward declarations |
| TD-020 | P2 | Dead code | ImpTimer, unused fonts/music/state/APIs/constants remain | Noise and misleading architecture | Remove after one call-site/build audit |
| TD-021 | P2 | Rendering | Player renders before map; Monster is a fixed global overlay | Possible occlusion/unclear layer intent | Define explicit render layers after visual regression check |
| TD-022 | P2 | Repository | No `.gitignore`; binaries/objects/vendor archives tracked; docs/build commands drift | Large repo and stale artifacts | Define artifact policy, ignore generated files, update canonical build docs |
| TD-023 | P2 | Naming | Misspellings and mixed heart/money/threat terminology | Search/API comprehension cost | Mechanical rename after behavior is covered |
| TD-024 | P3 | Modern C++ | `NULL`, macros, C-style patterns, unused parameters/fields | Style inconsistency and warnings | Incremental warning cleanup without gameplay changes |

## Highest-risk dependency clusters

- TD-001, TD-009, TD-010, and TD-013 all touch progression/timing; do not solve them in unrelated patches.
- TD-003 and TD-018 affect gameplay feel. Preserve explicit current hitbox intent before replacing the flawed implementation.
- TD-007 and TD-008 affect destruction order and ownership; change entities/caches only with a documented lifetime sequence.
- TD-011 and TD-019 should follow P0 fixes. Moving globals before behavior is stabilized would enlarge the regression surface.
