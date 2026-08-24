# Build and Run

## Supported configuration found in the repository

The packaged and verified build is Windows/MinGW. The source includes SDL headers and MinGW libraries under `src/`, and the required runtime DLLs are placed in the repository root beside `main.exe`. The Makefile also has a native Unix branch that obtains SDL2/image/ttf/mixer flags from `pkg-config`; that branch was dry-run verified but not compiled on this Windows host. No CMake project, Visual Studio solution, package manager manifest, installer, or CI build was found.

The current Makefile requires:

- A C++17-capable `g++`.
- GNU Make available as `make` if using the Makefile.
- On Windows, the checked-in `src/include`/`src/lib` contents and root SDL runtime DLLs.
- On native Unix-like systems, development packages exposing `sdl2`, `SDL2_image`, `SDL2_ttf`, and `SDL2_mixer` through `pkg-config`.

At startup the Windows build queries the first display's usable pixel bounds and chooses a centered window that preserves the 1422x800 logical canvas. The window is always shown in windowed mode and is resizable; it does not use an SDL fullscreen flag. On smaller laptop displays, SDL scales the logical canvas down to leave room for the taskbar and window decorations. Menu mouse coordinates are converted back to the logical canvas after scaling.

During the 2026-08-24 audit, MSYS2 `g++ 14.1.0` was available, but neither `make` nor `mingw32-make` was in PATH.

## Canonical build

If GNU Make is available in an MSYS-compatible shell:

```sh
make
```

The Makefile builds `main.exe` on Windows and `main` on native Unix-like systems from all 11 application translation units. Its `clean` recipe uses `rm`, so it assumes a shell that provides that command.

Equivalent direct PowerShell build, derived exactly from the current Makefile source list:

```powershell
g++ -Wall -std=c++17 -Isrc/include -static-libgcc -static-libstdc++ `
  src/main.cpp src/CommonFunc.cpp src/BaseObject.cpp src/gamemap.cpp `
  src/MainObject.cpp src/ImpTimer.cpp src/BulletObject.cpp `
  src/ThreatObject.cpp src/PlayHealth.cpp src/TextObject.cpp src/Profiler.cpp `
  -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer `
  -o main.exe
```

Run from the repository root because all runtime asset paths are relative:

```powershell
.\main.exe
```

Do not use the build commands in `README.md`, `README.jp.md`, `#READ_ME.txt`, or `#rungame.txt` as current commands: they omit `src/Profiler.cpp`, while current source calls profiler functions.

## Bundled dependency versions

| Component | Active header | Root DLL | Notes |
| --- | --- | --- | --- |
| SDL2 | 2.30.0 | 2.30.1 | Minor patch mismatch |
| SDL_image | 2.8.2 | 2.8.2 | Matches |
| SDL_mixer | 2.6.3 | 2.6.3 | An unrelated 2.8.0 archive also exists |
| SDL_ttf | 2.21.1 | 2.21.1 | An unrelated 2.22.0 archive also exists |

This table describes repository files, not a guarantee that every `.a`/import library has matching embedded metadata.

## Expected runtime assets

The executable needs the current working directory to contain `res/` with:

- `font/1.ttf`, `font/2.ttf`.
- Seven WAV files used by `src/main.cpp`.
- Background, menu, win, five journey, player, HUD, bullet, tile, and enemy images referenced in source.
- `pic/map/map01.txt`.

Required fonts, images, audio, map data, textures, and text caches are validated before entering the menu. Missing/corrupt assets now produce a nonzero startup exit after ordered cleanup. The map parser requires exactly 10,110 values and tile IDs 0-7; blank ID 0 does not allocate a texture.

## Verification result

The Windows branch was rebuilt with GNU Make and `-Wall -Wextra -Wpedantic` on 2026-08-24 with no diagnostics. The equivalent direct PowerShell build also passed. A non-Windows forced dry run selected `pkg-config` flags and omitted the MinGW libraries as intended; an actual Unix build remains unverified.

Step 1 later completed visible automated runs that opened the SDL window, selected Start in one stress run, entered the outer gameplay/death flow, and shut down with exit code 0. The automation did not complete the level or visually assert every input result; full gameplay, restart, and win acceptance remain **not confirmed from the current codebase**. See `../performance-baseline.md`.

## Tests and diagnostics

Focused regression sources now exist under `tests/` for collision, runtime allocation, map ownership/ranges, and profiler portability. They are compiled as standalone executables; there is no unified runner or CI. For a safe release, also:

1. Rebuild with `-Wall -Wextra -Wpedantic`.
2. Launch from the repository root.
3. Exercise menu start/exit, movement in both directions, jump, firing before and after moving, heart collection, player/enemy and bullet/enemy collision, every journey boundary, death/replay, win/replay, and window close.
4. Watch the five-second profiler output for unexpected runtime image/texture/font/sound loads.

The gameplay exercise steps require interactive validation; do not report them as passed unless actually exercised.

## Emscripten readiness boundary

Application source no longer includes Win32 headers or calls Win32 process APIs, and runtime timing/input/render/audio paths use SDL. This removes an immediate compiler dependency for future ports. The current nested menu/journey/game-over/win loops and blocking waits are not an Emscripten callback-style main loop, so a browser build is not claimed. Complete TD-010/R11 before adding an `em++` target and asset preloading flags.
