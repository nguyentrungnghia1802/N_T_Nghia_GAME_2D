# Project Overview

## What the game does

T-Kun's Journey is a side-view platform game with an automatically advancing horizontal camera/background. The player controls a dinosaur with `A`/`D`, jumps with `W`, and fires a horizontal projectile with the left mouse button (`src/MainObject.cpp::MainObject::HandelInputAction`). The map contains terrain, collectible heart tiles, and the four end-scene tiles. Four groups of 12 enemies are generated across successive map regions (`src/main.cpp::MakeThreats`).

The player starts with three displayed health icons. The fourth death enters the game-over screen (`src/main.cpp:304-363`). Collected hearts form the score; the highest score is retained only for the current process. Reaching the hard-coded end threshold sets the global `winner` flag and opens the win screen (`src/MainObject.cpp::MainObject::CheckToMap`). There is no save/load implementation.

## Major features present

- Mouse-driven start/exit menu.
- 1422x800 logical gameplay canvas rendered through an adaptive, resizable windowed SDL renderer.
- One 1011x10, 80-pixel tile map loaded from text.
- Frame-strip animation for player and enemies.
- Tile collision, enemy/player collision, and bullet/enemy collision.
- Heart collection, health icons, elapsed-time display, and process-local high score.
- WAV playback for map music and game events.
- Five journey image screens, a game-over overlay, and a win screen.
- Portable runtime counters for frame work, asset loads, entity work, and collisions.

## Technology stack

| Area | Technology found |
| --- | --- |
| Language | C++17 plus a small standalone C++ map conversion utility |
| Platform | Windows/MinGW is packaged and verified; native Unix SDL build flags are prepared but not verified here |
| Core | SDL2; bundled header reports 2.30.0 and root DLL reports 2.30.1 |
| Images | SDL_image 2.8.2 |
| Fonts | SDL_ttf; bundled header/DLL report 2.21.1 |
| Audio | SDL_mixer 2.6.3 in the active headers/DLL |
| Build | Hand-written Makefile and direct `g++` command |
| Map authoring | Tiled 1.10.2 `.tmx`/`.tsx` source plus a comma-to-space converter |
| Tests | Focused C++ regression executables under `tests/`; no unified runner/CI |

Archives under `SDL_Make_Pic/` include other SDL release packages, but they are source/material archives, not evidence that those versions are linked by the current build.

## Architectural summary

The runtime is a small object-oriented domain model under procedural orchestration:

- `BaseObject` wraps an SDL texture and rectangle.
- `MainObject` owns player simulation and active/pooled `unique_ptr` bullet lists.
- `ThreatsObject` owns enemy simulation state and borrows cached textures.
- `GameMap` owns the runtime/base map copies and tile textures.
- `PlayerPower`, `PlayerMoney`, and `TextObject` render HUD elements.
- `Profiler` counts work and logs at five-second intervals.
- `main.cpp` owns nearly all top-level state, resource initialization, screen loops, collision orchestration, and shutdown.

This is not an ECS. It has no general scene abstraction, component registry, event bus, physics engine, resource manager, save system, scripting layer, or networking.

## Important terminology

| Term | Meaning in this codebase |
| --- | --- |
| Map position | `Map::start_x_`/`start_y_`, used as the camera offset |
| World position | `x_pos_`/`y_pos_` in player/enemy objects |
| Screen rectangle | `rect_`, usually recomputed as world position minus map offset |
| Journey | One of five full-screen progress images intended at region boundaries |
| Threat | Enemy; code class is named `ThreatsObject` |
| Heart/money | The same collectible score concept; naming alternates between heart and money |
| Come-back time | Frame counter used to hide and reposition player/enemy after a death/fall |
| Active threat | Enemy within one screen-width margin of the current camera |

## Scope and non-features

Only one map is loaded. There is no confirmed multiplayer, battle thread, persistence, configurable controls, or multiple level loader. The window scales the fixed logical canvas to the display's usable pixel bounds; it does not change the map or gameplay coordinate system. README claims about multithreading are not supported by runtime source; no game thread is created.
