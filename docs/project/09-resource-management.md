# Resource Management

## Ownership model

The project combines explicit SDL cleanup with a small texture wrapper. `BaseObject` is the key texture-ownership mechanism:

- `LoadImg` destroys the previous owned texture, loads a temporary surface, creates a texture, frees the surface, and marks the texture owned.
- `UseTexture` frees prior state, stores a borrowed texture, and sets `owns_texture_ = false`.
- `Free` destroys only owned textures and always nulls the pointer.
- Copy construction and copy assignment are deleted, preventing two wrappers from owning the same texture.

This supports global texture caches while keeping owner and borrower destruction distinct.

## Resource inventory

| Resource | Creation | Owner | Borrowers/users | Destruction | Assessment |
| --- | --- | --- | --- | --- | --- |
| `SDL_Window* g_window` | `InitData` | `main.cpp` globals | SDL event/render setup | `close` | Explicit and clear |
| `SDL_Renderer* g_screen` | `InitData` | `main.cpp` globals | All rendering/loading | `close` | Destroyed after every texture owner |
| Background/Monster textures | `BaseObject::LoadImg` | global `BaseObject`s | direct render | explicit `Free` | Clear |
| Player/enemy/bullet cache textures | `LoadRuntimeTextures` | global `BaseObject`s | player, threats, bullets borrow | entities cleared, then caches freed | Correct order in `close` |
| Tile textures | `GameMap::LoadTiles` | `GameMap::tile_mat` objects | `DrawMap` | `GameMap::FreeTiles` before renderer; destructor fallback | Explicit and ordered |
| Player/HUD textures | `BaseObject::LoadImg` | player/HUD `BaseObject` bases | direct render | explicit `Free` | Clear; restart reloads health texture |
| Text textures | `TextObject::LoadFromRenderText` | each `TextObject` | text render | cache rebuild/destructor/`close` | Clear while fonts/renderer live |
| Menu texture/surface | `LoadFromFile`/`Call_Menu` | raw globals | menu loop | `FreeMenuResources` | Explicit; called on all visible menu exits |
| Win texture/surface | `LoadFromFile`/`Create_texture` | raw globals | win loop | `FreeWinResources` | Explicit |
| Journey textures/surfaces | same | raw globals | journey loop | `FreeJourneyResources` | Explicit |
| Temporary image surfaces | `BaseObject::LoadImg` | local | texture creation | same function | Clear |
| Temporary text surfaces/textures | `TextObject` or `renderText` | local/object | immediate draw/cache | immediate or object cleanup | Clear, but modal churn is costly |
| Six `TTF_Font*` | `LoadFromFile` | raw globals | text objects/functions borrow | `CloseFont` in `close` | Explicit; two are unused |
| Seven `Mix_Chunk*` | `LoadFromFile` | raw globals | direct playback | playback halt, `FreeChunk`, then `Mix_CloseAudio` | Explicit and ordered |
| `Mix_Music* gMusic` | never loaded | raw global | none | guarded `Mix_FreeMusic` | Dead state |
| Enemy objects | `MakeThreats` | `ThreatList` unique pointers | per-frame raw targets | erase/clear | Clear ownership |
| Bullet objects | mouse input `make_unique` | `MainObject::BulletList` | main collision loop borrows with `.get()` | vector erase/clear | Explicit unique ownership |

## Resolved lifecycle risks

### Tile texture destruction after renderer shutdown

`close` now calls `game_map.FreeTiles()` before destroying `g_screen`. The `GameMap` destructor calls the same idempotent cleanup as a fallback, so global destruction only sees null texture handles.

### Audio device not explicitly closed

Successful `Mix_OpenAudio` is tracked. Shutdown halts channels/music, frees chunks/music, calls `Mix_CloseAudio`, then quits SDL_mixer and SDL.

### Shallow-copyable texture owners

`BaseObject` copy construction and assignment are deleted. Current derived owners are not copied by value, and the build enforces that constraint.

### Projectile ownership across replay

`MainObject` stores bullets in `vector<unique_ptr<BulletObject>>`. Erase, clear, shutdown, and replay release projectiles automatically; the removed public raw-pointer setter can no longer duplicate ownership.

## Remaining resource-related correctness risk

### Unchecked partial initialization

`LoadFromFile` does not return success. A null menu surface is dereferenced for `g_img_menu->w/h`; null win/journey surfaces are similarly dereferenced in `Create_texture`. Null fonts/chunks/textures can be passed into SDL APIs. This is a correctness/crash risk during missing/corrupt asset or decoder failure.

### Borrowed texture lifetime

Player, enemy, and bullet textures are borrowed. The current `close` order clears threats/bullets and frees the player's borrowed pointer before freeing caches, which is correct. Preserve that ordering. Restart must never reload/free global caches while entities still reference them.

## Recommended ownership target

For this small game, use narrow RAII rather than a large resource framework:

1. Wrap window, renderer, textures, surfaces, fonts, chunks, and audio-device state with small custom deleters when a top-level application owner is introduced.
2. Make `LoadFromFile` return a detailed success/failure result and stop before dereferencing missing resources.
