# Repository Structure

## Logical tree

```text
N_T_Nghia_GAME_2D/
├── Makefile                     # Canonical current source list and link flags
├── main.exe                     # Tracked prebuilt artifact; may lag source HEAD
├── SDL2*.dll, lib*.dll          # Runtime DLLs shipped beside the executable
├── README.md / README.jp.md     # User-facing game descriptions; build line is stale
├── #READ_ME.txt                 # Older bilingual instructions; build line is stale
├── #rungame.txt                 # Older direct build command; missing Profiler.cpp
├── docs/
│   ├── agent.md                 # Repository workflow rules
│   ├── refact.md                # Historical roadmap based mostly on older code
│   └── project/                 # Current architecture/audit knowledge base
├── src/
│   ├── main.cpp                 # Entry point, global state, all top-level flows
│   ├── CommonFunc.*             # Shared constants/types/globals and collision helper
│   ├── WindowConfig.h           # Pure adaptive window-size calculation
│   ├── BaseObject.*             # Texture/rectangle wrapper and ownership flag
│   ├── gamemap.*                # Map load/reset/draw and tile texture ownership
│   ├── MainObject.*             # Player input, physics, bullets, tile interaction
│   ├── ThreatObject.*           # Enemy animation, movement, map collision
│   ├── BulletObject.*           # Horizontal projectile movement
│   ├── PlayHealth.*             # Health and heart HUD icons
│   ├── TextObject.*             # Cached TTF text texture
│   ├── Profiler.*               # Runtime counters/logging
│   ├── ImpTimer.*               # Unused legacy timer implementation
│   ├── include/SDL2/            # Vendored SDL family headers
│   ├── lib/                     # Vendored MinGW import/static libraries and metadata
│   └── *.o                      # Tracked generated object files
├── res/
│   ├── font/                    # Two runtime TTF files
│   ├── Music/                   # WAV music/effects; some files are not referenced
│   ├── pic/img/                 # Player, bullet, HUD, and background images
│   ├── pic/threats/             # Enemy frame strips and Monster overlay
│   ├── pic/menu/                # Runtime menu plus source/reference artwork
│   ├── pic/journey/             # Five full-screen journey panels
│   ├── pic/map/                 # Runtime tiles, map text, win image, source artwork
│   │   └── TILE_MAP/            # Tiled project, tilesets, images, and converter tool
│   └── pic_for_rm/              # README screenshots; not loaded by runtime
├── SDL_Make_Pic/                # Large art workspace and SDL archives; not runtime code
└── .vscode/                     # IntelliSense configuration; no launch task
```

## Source versus generated/vendor material

The application-owned source is the 11 `.cpp` files and their headers directly under `src/`. `src/include/SDL2/` and `src/lib/` are third-party distribution contents and should not be edited as application code. `*.o`, `main.exe`, the map-builder executable, root DLLs, ZIP/RAR archives, and many art source files are binary/generated/distribution artifacts.

The repository has no `.gitignore`. Generated objects/executables and about 335 MB across 384 tracked files are committed. This increases clone size and makes it easy for binary artifacts to drift from source.

## Runtime assets versus source/reference assets

Runtime paths are hard-coded in `src/main.cpp`, `src/gamemap.cpp`, and `src/PlayHealth.cpp`. Assets under `pic_for_rm/`, most alternate art under `pic/img`, `pic/menu`, and `pic/map`, and `res/Music/Game_Start.wav` are not referenced by current runtime code. Do not delete them solely on that basis: some are README content or map/art source material.

The runtime map is `res/pic/map/map01.txt`. It contains exactly 10,110 integers (10x1011) with IDs 0-7. It differs from `TILE_MAP/Build Map/map.txt`, `input.txt`, and `SDL_Make_Pic/Map/map01.txt`; none of those should be assumed canonical for runtime.

## Configuration and tests

- Compile configuration: `Makefile` and `.vscode/c_cpp_properties.json`.
- Runtime configuration files: none; gameplay constants are compile-time macros/constants.
- Tests: focused application regressions live under `tests/`; bundled `SDL2_test` headers/libraries are third-party, not project tests.
- CI/package scripts: none found.

