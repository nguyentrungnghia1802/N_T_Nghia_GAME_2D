# File Reference

## Application source

| File | Responsibility | Important dependencies | Notes |
| --- | --- | --- | --- |
| `src/main.cpp` | Entry point, resources, screen loops, gameplay orchestration, threat factory, shutdown | All app modules and SDL libraries | Primary control-flow file; 1219 lines |
| `src/CommonFunc.h` | Shared SDL includes, constants, `Input`, `Map`, extern globals, helper declarations | SDL2/image/ttf/mixer, currently `TextObject.h` | God header and circular include source |
| `src/CommonFunc.cpp` | Shared global definitions, explicit hitbox builders, AABB collision, and mouse focus helpers | Profiler | Collision footprints are deliberately retained for gameplay compatibility |
| `src/BaseObject.h` | Texture/rect owning-or-borrowing base class | `CommonFunc.h` | Noncopyable; runtime ownership flag distinguishes owner/borrower |
| `src/BaseObject.cpp` | Image load, texture borrow/render/free, viewport test | SDL_image, profiler | `Render1` repeats scrolling background |
| `src/gamemap.h` | `TileMat`, `GameMap`, visible range/reset APIs | `Map`, `BaseObject` | Broken include guard; `getMap` by value |
| `src/gamemap.cpp` | Map parser, tile loading/drawing, camera/reset/base map | Globals `winner`, `map_start` | Duplicate unused loader; owns tile textures |
| `src/MainObject.h` | Player interface/state and active/pooled bullet owner | `BaseObject`, `BulletObject`, `Map` | Mixed responsibilities; explicit `unique_ptr` ownership |
| `src/MainObject.cpp` | Input, animation, player/bullet update/render, tile collision, score/win/death | Audio globals, profiler | Highest-risk gameplay file |
| `src/BulletObject.h` | Bullet direction/speed/active state and reuse reset | `BaseObject` | Deterministic right-facing default |
| `src/BulletObject.cpp` | Horizontal movement and screen deactivation | SDL rectangle from base | Y movement/border unused |
| `src/ThreatObject.h` | Enemy movement/texture/animation interface/state | `BaseObject`, `Map` | Global appearance coupling remains |
| `src/ThreatObject.cpp` | Enemy update/render, tile collision, patrol and texture switch | Globals, profiler | Chained predicate bug; active filtering is outside |
| `src/PlayHealth.h` | Health and heart HUD classes | `BaseObject` | Broken include guard |
| `src/PlayHealth.cpp` | HUD image load, positions, render/decrement | SDL renderer | `pop_back` assumes nonempty list |
| `src/TextObject.h` | Cached text texture API/state | `CommonFunc`, `BaseObject` | Unnecessary BaseObject include/inheritance absent |
| `src/TextObject.cpp` | TTF render cache and render/free | SDL_ttf, profiler | Clip height uses width |
| `src/Profiler.h` | Counter API | `<cstdint>` | No enable/disable configuration |
| `src/Profiler.cpp` | Frame/load counters and five-second log | SDL timers; Win32 process APIs under `_WIN32` | Frame work excludes cap delay |
| `src/ImpTimer.h/.cpp` | Legacy tick timer | SDL through `CommonFunc` | No application call sites; dead-code candidate |

## Build, configuration, and documentation

| File/directory | Responsibility | Notes |
| --- | --- | --- |
| `Makefile` | Current app source list, flags, link libraries, target | Canonical build definition; `clean` assumes `rm` |
| `.vscode/c_cpp_properties.json` | IntelliSense compiler/standard | Points at MSYS2 UCRT64 GCC and C++17 |
| `.vscode/launch.json` | Debug config placeholder | No launch configuration |
| `README.md`, `README.jp.md`, `#READ_ME.txt` | Player/project descriptions | Build commands omit `Profiler.cpp`; some architecture claims are stale |
| `#rungame.txt` | Legacy one-line build | Stale |
| `docs/agent.md` | Repository workflow constraints | Follow for future implementation tasks |
| `docs/refact.md` | Historical 10-step refactor plan | Several “current” findings describe code before commits 1-10 |
| `docs/project/` | Current verified knowledge base | Prefer this for HEAD architecture |

## Runtime data and assets

| Path | Responsibility | Notes |
| --- | --- | --- |
| `res/pic/map/map01.txt` | Canonical runtime 1011x10 tile IDs | Exactly 10,110 IDs; differs from source/export copies |
| `res/pic/map/0.png` ... `7.png` | Runtime tile textures | ID 0 is loaded but blank tiles are not rendered; IDs 1-7 used |
| `res/pic/map/TILE_MAP/untitled.tmx` | Tiled 1.10.2 authoring map | Same declared 1011x10/80x80 geometry |
| `res/pic/map/TILE_MAP/*.tsx` | Tiled tileset descriptors | One image per tileset |
| `res/pic/map/TILE_MAP/Build Map/main.cpp` | CSV comma-to-space converter | Not in game build; output is not runtime map |
| `res/pic/img/player_*1.png` | Six-frame player strips | Left height 91, right height 96 |
| `res/pic/threats/threat_*.png` | Five-frame enemy strips | Four enemy regions; types 2/3 have left/right variants |
| `res/pic/img/fire.png` | Borrowed bullet texture | 63x63 |
| `res/pic/img/player_pw.png`, `heart_.png` | HUD icons | Owned by HUD objects |
| `res/pic/img/background.jpg` | Repeating background | Exactly 1422x800 |
| `res/pic/menu/menu.png` | Menu image | 1422x826 rendered to its own size in 1422x800 window |
| `res/pic/journey/*.png` | Five journey panels | Each 1422x800 |
| `res/pic/map/WIN_GAME.png` | Win background | 1422x800 |
| `res/pic/threats/Monster.png` | Fixed render overlay | 202x765, default rect position 0,0 |
| `res/font/*.ttf` | Runtime fonts | Six handles opened from two files at different sizes |
| `res/Music/*.wav` | Audio effects/music | `Game_Start.wav` is not runtime-referenced; `Start.wav` is |
| `res/pic_for_rm/` | README screenshots | Documentation assets, not runtime loads |

## Vendored/generated material

| Path | Role | Change guidance |
| --- | --- | --- |
| `src/include/SDL2/` | Third-party SDL headers | Do not edit as application source |
| `src/lib/` | Third-party MinGW SDL libraries/metadata | Replace only as a coordinated dependency upgrade |
| Root `SDL2*.dll`/runtime DLLs | Packaged runtime dependencies | Keep versions compatible with linked import libraries |
| `src/*.o`, `main.exe` | Generated/packaged build artifacts | Rebuild explicitly; do not infer source status from timestamps alone |
| `SDL_Make_Pic/` | Art/dependency archive workspace | Not runtime; very large and partly duplicated |
