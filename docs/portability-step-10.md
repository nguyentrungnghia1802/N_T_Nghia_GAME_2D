# Step 10 Portability Report

Completed on 2026-08-24.

## Changes

- Removed direct `windows.h`, `psapi.h`, and `tlhelp32.h` dependencies from application source.
- Removed Win32-only CPU, working-set, and thread-snapshot queries from the in-process profiler instead of emitting misleading zeros on other platforms.
- Retained portable SDL performance timing, entity/collision/load counters, console output, and `GAME_PROFILE_LOG` capture.
- Updated the Makefile to select the checked-in MinGW SDL bundle on Windows or SDL2 family flags from `pkg-config` on native Unix-like systems.
- Enabled `-Wall -Wextra -Wpedantic` in the canonical Makefile build.

## Verification

- Windows GNU Make rebuild: passed with no diagnostics.
- Makefile non-Windows forced dry run: selected `pkg-config`-style flags, a suffixless `main` target, and no MinGW libraries.
- Application-source audit excluding vendored SDL headers: no Win32 header/API references.
- `tests/profiler_portability_tests.cpp`: emitted a five-second profiler record with retained engine counters and no Win32-only fields.
- Runtime startup and SDL quit smoke: exited with code 0.

## Remaining portability boundary

An actual Unix build was not available on this Windows host. Emscripten is not claimed: the nested modal loops and blocking waits must first become a callback-compatible state loop (TD-010/R11). The SDL/standard-C++ source and build separation completed here remove the direct OS dependency that would otherwise block that work.
