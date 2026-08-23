# Technical-Debt Inventory

Severity uses P0 for correctness/crash/memory-safety work, P1 for major maintainability/timing issues, P2 for architecture/duplication, and P3 for optional cleanup.

| ID | Severity | Area | Problem | Impact | Suggested fix |
| --- | --- | --- | --- | --- | --- |
| TD-001 | P0 | Progression | Journey uses unreachable exact camera equalities | Journey panels and threat-style toggles do not occur in normal scrolling | Detect crossing/ranges and record one-shot journey progress |
| TD-002 | P0 (resolved) | Input/bullets | Initial player facing and `bullet_dir_` left first-shot direction indeterminate | Fixed in Step 4 with deterministic right-facing defaults | Preserve explicit direction on every shot |
| TD-003 | P0 (resolved) | Collision | Step 7 replaced corner tests with tested AABB queries and explicit legacy-sized hitboxes | Containment/crossing work without changing difficulty | Preserve edge, containment, directional-overlap, and footprint tests |
| TD-004 | P0 (resolved) | Enemy physics | Chained enemy tile predicate | Corrected in Step 4 | Retain focused tile-collision regression coverage |
| TD-005 | P0 (resolved) | Startup | Asset/map failures were ignored before pointer/index use | Step 4 validates required startup resources and fails cleanly | Preserve aggregated startup checks |
| TD-006 | P0 (resolved) | Headers | Three include guards defined the wrong macro | Corrected and double-include tested in Step 4 | Keep header self-containment checks |
| TD-007 | P0 (resolved) | SDL lifetime | Tile textures outlived renderer shutdown; audio device was not closed | Fixed in Step 2 with ordered tile/audio cleanup | Preserve documented shutdown order |
| TD-008 | P0 (resolved) | Ownership | `BaseObject` was shallow-copyable despite owning raw textures | Fixed in Step 2 by deleting copy operations | Add safe move only if a real use requires it |
| TD-009 | P1 (resolved) | Timing | `delta_time` was unused; simulation was frame-based; cap targeted 16 ms | Step 5 applies consistent 60 Hz-normalized delta scaling and a fractional 16.667 ms cap | Preserve split-step equivalence tests |
| TD-010 | P1 | State flow | Menu/journey/game-over/win are nested modal loops | Duplicated input/timing; blocks portable main loop | Central state-driven loop with enter/update/render/exit |
| TD-011 | P1 | Architecture | `main.cpp` owns nearly everything through globals | High change risk and poor testability | Extract a small top-level `Game`/resource owner incrementally |
| TD-012 | P1 (resolved) | Bullets | Step 8 uses active/pooled `unique_ptr` containers and index-safe recycling | Safe lifetime and reuse after peak concurrency | Preserve pointer-reuse and shifted-element behavior tests |
| TD-013 | P1 | Time/score | Initial timer includes menu; replay/reset semantics are inconsistent | Displayed completion time is misleading | Define play-time semantics and update only in Playing state |
| TD-014 | P1 (resolved) | Map | Step 9 uses one `GameMap`-owned runtime instance through a stable reference | Removed two full map copies per frame | Preserve reference-stability and restart-restoration tests |
| TD-015 | P1 (resolved) | UI performance | Static modal text was rasterized/uploaded every iteration | Fixed in Step 3 with startup/state-entry text caches | Preserve cache-only modal rendering |
| TD-016 | P1 (resolved) | Menu performance | Menu/modal loops had no shared pacing | Step 6 applies the fractional 60 Hz cap to all static loops | Preserve settled-menu CPU measurement |
| TD-017 | P2 (resolved) | Map safety | Parser did not verify 10,110 reads or tile range before indexing | Step 4 validates exact count and IDs 0-7; drawing also bounds-checks IDs | Preserve malformed-map tests |
| TD-018 | P2 | Duplication | Player/enemy tile collision and map loaders duplicate logic | Divergent fixes and subtle bugs | Share small tile-query/predicate helpers; remove unused loader |
| TD-019 | P2 | Includes | `CommonFunc.h`/`TextObject.h` cycle and broad SDL/global exposure | Fragile includes and slow/broad recompilation | Split constants/types/globals; use direct includes/forward declarations |
| TD-020 | P2 | Dead code | ImpTimer, unused fonts/music/state/APIs/constants remain | Noise and misleading architecture | Remove after one call-site/build audit |
| TD-021 | P2 | Rendering | Player renders before map; Monster is a fixed global overlay | Possible occlusion/unclear layer intent | Define explicit render layers after visual regression check |
| TD-022 | P2 | Repository | No `.gitignore`; binaries/objects/vendor archives tracked; docs/build commands drift | Large repo and stale artifacts | Define artifact policy, ignore generated files, update canonical build docs |
| TD-023 | P2 | Naming | Misspellings and mixed heart/money/threat terminology | Search/API comprehension cost | Mechanical rename after behavior is covered |
| TD-024 | P3 (partial) | Modern C++ | `NULL`, macros, and C-style patterns remain | Step 4 removed all `-Wall -Wextra -Wpedantic` warnings; style modernization remains | Continue only as narrow mechanical cleanup |
| TD-025 | P2 (resolved) | Portability | Step 10 removed direct Win32 runtime APIs and added platform-selecting build flags | Application source is SDL/standard C++; native Unix compile remains unverified | Keep Win32 APIs outside runtime source; finish TD-010 before Emscripten |

## Highest-risk dependency clusters

- TD-001, TD-009, TD-010, and TD-013 all touch progression/timing; do not solve them in unrelated patches.
- TD-018 affects gameplay feel. Step 7 captured the intended hitbox footprints before resolving TD-003.
- TD-007 and TD-008 affect destruction order and ownership; change entities/caches only with a documented lifetime sequence.
- TD-011 and TD-019 should follow P0 fixes. Moving globals before behavior is stabilized would enlarge the regression surface.
