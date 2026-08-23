# Step 3 Runtime Asset Cache Report

Date: 2026-08-24

## Completed changes

- Added startup-owned textures for the health and heart HUD images; replay now resets HUD state without `IMG_Load` or texture creation.
- Converted menu, win, and journey surfaces to textures before entering the menu, then released the source surfaces immediately.
- Preloaded white/red menu label variants so hover changes reuse textures.
- Replaced immediate game-over, win, and time-limit text rendering with retained `TextObject` caches.
- Removed two duplicate, unused font opens.

## Verification

- Full C++17 build with the Makefile source list: passed.
- Automated menu-to-gameplay startup and graceful close: passed with exit code 0.
- First gameplay profiler interval reported 28 startup image loads, 45 startup texture creates, four font loads, seven sound loads, and 17 text renders. The larger startup texture count is intentional preloading; font opens decreased from six to four.
- Source audit confirms all `LoadImg`, `IMG_Load`, `TTF_OpenFont`, and `Mix_LoadWAV` call sites are confined to startup loaders. Replay uses cached HUD textures.
- Source audit confirms modal loops now call only cached `RenderText`; `LoadFromRenderText` for dynamic modal values occurs once before each loop.

The existing dynamic HUD timer creates a new text texture when its displayed second changes. This is content-driven cache invalidation, not repeated rendering of unchanged text. A glyph atlas would be a broader allocation/rendering change and remains outside Step 3.
