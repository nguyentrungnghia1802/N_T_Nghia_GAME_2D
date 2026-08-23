# Build and Run

## Supported configuration found in the repository

The packaged build is Windows/MinGW. The source includes SDL headers and MinGW libraries under `src/`, and the required runtime DLLs are placed in the repository root beside `main.exe`. No CMake project, Visual Studio solution, package manager manifest, installer, or CI build was found.

The current Makefile requires:

- A C++17-capable `g++`.
- GNU Make available as `make` if using the Makefile.
- The checked-in `src/include` and `src/lib` contents.
- Root SDL/runtime DLLs when running the dynamically linked SDL family libraries.

During the 2026-08-24 audit, MSYS2 `g++ 14.1.0` was available, but neither `make` nor `mingw32-make` was in PATH.

## Canonical build

If GNU Make is available in an MSYS-compatible shell:

```sh
make
```

The Makefile builds `main.exe` from all 11 application translation units. Its `clean` recipe uses `rm`, so it assumes a shell that provides that command.

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

Required fonts, images, audio, map data, textures, and text caches are validated before entering the menu. Missing/corrupt assets now produce a nonzero startup exit after ordered cleanup. The map parser requires exactly 10,110 values and tile IDs supported by the loaded tile cache.

## Verification result

The Makefile-equivalent direct build completed with exit code 0 on 2026-08-24. `-Wall -Wextra -Wpedantic -fsyntax-only` also completed with warnings. Confirmed warnings include the single `&` in `MainObject::set_clips`, chained `!=` in `ThreatsObject::CheckToMap`, signed/unsigned loop comparisons, unused parameters/variables, and omitted `SDL_Color::a` initializers.

Step 1 later completed visible automated runs that opened the SDL window, selected Start in one stress run, entered the outer gameplay/death flow, and shut down with exit code 0. The automation did not complete the level or visually assert every input result; full gameplay, restart, and win acceptance remain **not confirmed from the current codebase**. See `../performance-baseline.md`.

## Tests and diagnostics

There is no project test suite. For a safe change, at minimum:

1. Rebuild with `-Wall -Wextra -Wpedantic`.
2. Launch from the repository root.
3. Exercise menu start/exit, movement in both directions, jump, firing before and after moving, heart collection, player/enemy and bullet/enemy collision, every journey boundary, death/replay, win/replay, and window close.
4. Watch the five-second profiler output for unexpected runtime image/texture/font/sound loads.

The last three steps require interactive validation; do not report them as passed unless actually exercised.
