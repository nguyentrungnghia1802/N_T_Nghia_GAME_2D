# Resource Management

## Ownership model

The project combines manual raw-pointer cleanup with a small texture wrapper. `BaseObject` is the key ownership mechanism:

- `LoadImg` destroys the previous owned texture, loads a temporary surface, creates a texture, frees the surface, and marks the texture owned.
- `UseTexture` frees prior state, stores a borrowed texture, and sets `owns_texture_ = false`.
- `Free` destroys only owned textures and always nulls the pointer.

This supports global texture caches, but ownership is implicit and copy operations are unsafe.

## Resource inventory

| Resource | Creation | Owner | Borrowers/users | Destruction | Assessment |
| --- | --- | --- | --- | --- | --- |
| `SDL_Window* g_window` | `InitData` | `main.cpp` globals | SDL event/render setup | `close` | Explicit and clear |
| `SDL_Renderer* g_screen` | `InitData` | `main.cpp` globals | All rendering/loading | `close` | Explicit; destroyed before tile owners |
| Background/Monster textures | `BaseObject::LoadImg` | global `BaseObject`s | direct render | explicit `Free` | Clear |
| Player/enemy/bullet cache textures | `LoadRuntimeTextures` | global `BaseObject`s | player, threats, bullets borrow | entities cleared, then caches freed | Correct order in `close` |
| Tile textures | `GameMap::LoadTiles` | `GameMap::tile_mat` objects | `DrawMap` | only global destructors | Unsafe shutdown order |
| Player/HUD textures | `BaseObject::LoadImg` | player/HUD `BaseObject` bases | direct render | explicit `Free` | Clear; restart reloads health texture |
| Text textures | `TextObject::LoadFromRenderText` | each `TextObject` | text render | cache rebuild/destructor/`close` | Clear while fonts/renderer live |
| Menu texture/surface | `LoadFromFile`/`Call_Menu` | raw globals | menu loop | `FreeMenuResources` | Explicit; called on all visible menu exits |
| Win texture/surface | `LoadFromFile`/`Create_texture` | raw globals | win loop | `FreeWinResources` | Explicit |
| Journey textures/surfaces | same | raw globals | journey loop | `FreeJourneyResources` | Explicit |
| Temporary image surfaces | `BaseObject::LoadImg` | local | texture creation | same function | Clear |
| Temporary text surfaces/textures | `TextObject` or `renderText` | local/object | immediate draw/cache | immediate or object cleanup | Clear, but modal churn is costly |
| Six `TTF_Font*` | `LoadFromFile` | raw globals | text objects/functions borrow | `CloseFont` in `close` | Explicit; two are unused |
| Seven `Mix_Chunk*` | `LoadFromFile` | raw globals | direct playback | `FreeChunk` in `close` | Explicit; audio device not closed |
| `Mix_Music* gMusic` | never loaded | raw global | none | `Mix_FreeMusic(NULL)` | Dead state |
| Enemy objects | `MakeThreats` | `ThreatList` unique pointers | per-frame raw targets | erase/clear | Clear ownership |
| Bullet objects | mouse input `new` | `MainObject` by convention | main collision loop | manual delete | Fragile but mostly centralized |

## Confirmed/credible lifecycle risks

### Tile texture destruction after renderer shutdown

`game_map` is a global object. `close` destroys `g_screen` but never calls a `GameMap` cleanup. Its 20 `TileMat` elements are destroyed only during global teardown after `main` returns; their `BaseObject` destructors call `SDL_DestroyTexture`. This violates the intended “destroy textures before renderer/SDL” ordering. SDL renderer destruction also invalidates associated textures, so later destruction can operate on invalid handles. A crash on this exact runtime was **not confirmed from the current codebase**, but the ownership order is incorrect.

### Audio device not explicitly closed

`Mix_OpenAudio` succeeds during init, but `close` calls `Mix_Quit`/`SDL_Quit` without `Mix_CloseAudio`. Add explicit audio-device closure before quitting mixer/SDL.

### Unchecked partial initialization

`LoadFromFile` does not return success. A null menu surface is dereferenced for `g_img_menu->w/h`; null win/journey surfaces are similarly dereferenced in `Create_texture`. Null fonts/chunks/textures can be passed into SDL APIs. This is a correctness/crash risk during missing/corrupt asset or decoder failure.

### Shallow-copyable texture owners

`BaseObject` defines a destructor but not copy/move operations. The implicit copy constructor/assignment copy `p_object_` and `owns_texture_`, allowing two objects to believe they own one texture. No current application call site was found that copies a `BaseObject`-derived instance, so a current double-free is **not confirmed from the current codebase**. The type should still be made noncopyable or given correct move semantics before future container/value use.

### Borrowed texture lifetime

Player, enemy, and bullet textures are borrowed. The current `close` order clears threats/bullets and frees the player's borrowed pointer before freeing caches, which is correct. Preserve that ordering. Restart must never reload/free global caches while entities still reference them.

## Recommended ownership target

For this small game, use narrow RAII rather than a large resource framework:

1. Make `BaseObject` noncopyable and movable, or replace its flag with distinct owning/borrowing handle types.
2. Add `GameMap::FreeTiles` or rely on a top-level owner whose members destruct before renderer shutdown.
3. Wrap window, renderer, textures, surfaces, fonts, chunks, and audio-device state with small custom deleters.
4. Make `LoadFromFile` return a detailed success/failure result and stop before dereferencing missing resources.
5. Store bullets as `unique_ptr` (or values if address stability is not required).
6. Close audio with `Mix_CloseAudio` before `Mix_Quit`/`SDL_Quit`.

