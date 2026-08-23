# Performance Baseline

Baseline date: 2026-08-24

Branch baseline: `codex/refactor/full-optimization` at parent commit `95dc9d8` plus Step 1 profiler instrumentation.

This is the baseline for the task sequence in `docs/task.md`. It is not a claim that final optimization acceptance has passed.

## Test machine

| Component | Value |
| --- | --- |
| OS | Windows 11 Home 64-bit, 10.0.26200 |
| CPU | Intel Core i5-12500H, 12 cores / 16 logical processors |
| RAM | 15.71 GiB visible memory |
| GPUs | Intel Iris Xe; NVIDIA GeForce RTX 3050 Laptop GPU |
| Compiler | MSYS2 `g++ 14.1.0` |
| Window | 1422x800, accelerated SDL renderer, no vsync flag |

CPU percentages below are normalized to the entire 16-logical-processor machine. “Single-core equivalent” multiplies the normalized percentage by 16 to make a busy single-threaded loop easier to recognize. Working set is process-resident memory, not proof of heap ownership or leak freedom.

## Instrumentation added in Step 1

`src/Profiler.cpp` now reports normalized process `cpu_pct` beside the existing FPS, frame time, entity, render, collision, asset, text, RAM, and thread counters.

Profiler lines can also be written to a file without changing the normal game path:

```powershell
$env:GAME_PROFILE_LOG = "$PWD\profile.log"
.\main.exe
Remove-Item Env:GAME_PROFILE_LOG
```

If the variable is absent, output remains console-only. The file is truncated when profiling initializes and flushed once per five-second log interval.

## Build baseline

The full current source list built successfully with:

```text
-Wall -Wextra -Wpedantic -std=c++17
```

No new warning was introduced by Step 1. Existing warnings remain, including the suspicious `&` in `MainObject::set_clips`, chained `!=` in enemy tile collision, signed/unsigned comparisons, unused values/parameters, and omitted `SDL_Color::a` initializers. Fixing them belongs to later scoped tasks.

## Scenario A — Settled menu idle

Procedure:

1. Launch the freshly built executable from the repository root.
2. Wait three seconds after the SDL window appears so startup asset loading settles.
3. Leave the menu untouched for 5.30 seconds.
4. Close through the window close request.

| Metric | Baseline |
| --- | ---: |
| Normalized CPU | 7.23% of machine |
| Single-core equivalent | 115.66% |
| Working-set start | 126,768 KiB |
| Working-set end | 127,752 KiB |
| Working-set range | 126,768–127,752 KiB |
| Shutdown | Exit code 0 |

Interpretation: the menu consumes roughly one logical core or slightly more across renderer/audio/driver work. `Call_Menu` has no frame cap and the renderer was not created with vsync, so this is a confirmed CPU baseline for later state-loop/idle optimization.

## Scenario B — Automated transition stress

Procedure:

1. Launch and select Start after the menu is available.
2. Allow the four-second start transition.
3. Send `D`, one fire click, and one `W` input.
4. Observe the gameplay/death flow for 16.45 seconds.
5. Close with `WM_CLOSE`; shutdown returned exit code 0.

The automation did not complete the level. It entered the game-over modal, so the two profiler intervals represent different states and must not be averaged together.

### Interval 1 — Gameplay with death delay

| Metric | Baseline |
| --- | ---: |
| Frames / interval FPS | 184 / 36.69 FPS |
| Estimated minimum FPS | 1.00 |
| Frame time min / avg / max | 0.36 / 11.49 / 999.80 ms |
| Entity updates / frame | 3.45 |
| Entity renders / frame | 2.10 |
| Collision checks / frame | 3.08 |
| Image loads | 28 |
| Texture creates | 39 |
| Font loads | 6 |
| Sound loads | 7 |
| Text renders | 11 |
| Process CPU | 0.21% normalized |
| Working set | 145,320 KiB |
| Threads | 24 |

The approximately one-second maximum is consistent with the pumped death wait being inside an outer profiled frame. Startup load counters intentionally appear in the first interval because counters begin before asset loading and are reset after the first gameplay log.

### Interval 2 — Game-over modal returned to outer frame

| Metric | Baseline |
| --- | ---: |
| Frames / interval FPS | 88 / 7.03 FPS |
| Estimated minimum FPS | 0.10 |
| Frame time min / avg / max | 0.37 / 126.28 / 10,053.16 ms |
| Entity updates / frame | 4.58 |
| Entity renders / frame | 2.97 |
| Collision checks / frame | 5.12 |
| Runtime image/font/sound loads | 0 / 0 / 0 |
| Texture creates / text renders | 9,899 / 9,899 |
| Process CPU | 3.56% normalized |
| Working set | 141,200 KiB |
| Threads | 34 |

The 10-second maximum does not mean one normal gameplay render took 10 seconds. The profiler's outer frame remained open while the nested game-over loop ran. That loop repeatedly calls immediate `renderText`, which is directly confirmed by the 9,899 text renders and texture creations.

### External process sample across Scenario B

| Metric | Baseline |
| --- | ---: |
| Sample duration | 16.45 s |
| Normalized CPU | 2.74% |
| Single-core equivalent | 43.84% |
| Working-set start / end | 142,648 / 141,200 KiB |
| Working-set min / max | 139,708 / 146,252 KiB |
| Peak working set | 146,252 KiB |
| Shutdown | Exit code 0 |

No monotonic RAM growth is visible in this short sample. This is not sufficient to accept “no leak”; repeated restart/win cycles and a longer run remain required by Step 2/final acceptance.

## Before/after comparison ledger

| Metric/problem | Before (Step 1 baseline) | After optimization | Target step |
| --- | --- | --- | --- |
| Settled menu CPU | 7.23% machine / 115.66% single-core equivalent | Pending | Step 5/6 |
| Gameplay/death interval max frame | 999.80 ms | Pending | Step 5 |
| Game-over outer-frame max | 10,053.16 ms | Pending | Step 5 |
| Game-over text/texture creates | 9,899 in one modal interval | Pending | Step 3/8 |
| Runtime image loads after first interval | 0 observed | Revalidate | Step 3 |
| Active entity updates/renders | 3.45/2.10 then 4.58/2.97 per outer frame | Revalidate | Step 6 |
| Collision checks | 3.08 then 5.12 per outer frame | Revalidate | Step 7 |
| Working-set range in stress sample | 139,708–146,252 KiB | Pending long-run comparison | Step 2 |
| Clean shutdown | Exit code 0 in measured menu/stress runs | Revalidate repeatedly | Step 2/final |

“After” remains pending until the relevant task is implemented and the same scenario is rerun. Do not fill it with inferred or estimated improvements.

## Measurement limitations

- Automated input did not validate every movement/collision outcome visually and did not reach win/replay.
- The transition stress scenario entered game over; it is not steady-state gameplay.
- The outer-frame profiler cannot time nested modal iterations individually.
- CPU/GPU frequency, thermal state, background applications, and GPU selection were not controlled.
- GPU utilization and VRAM were not measured.
- No low-end machine was available in this run.
- A 16-second memory window cannot prove absence of leaks.
- `fps_min_est` is derived from the largest measured outer-frame duration and is not a percentile low.

These limitations are part of the baseline. Future comparisons must use the same state/scenario or clearly identify a new one.
