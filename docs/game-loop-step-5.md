# Step 5 Game-Loop Timing Report

Date: 2026-08-24

## Timing contract

The existing gameplay constants define behavior at 60 Hz. Each outer frame computes a clamped elapsed time and converts it to a nominal-frame scale:

```text
frame_scale = clamped_delta_seconds * 60
```

At `frame_scale == 1`, player/enemy acceleration and movement, bullet speed, camera/background scrolling, animations, and short frame counters retain their previous values.

## Completed changes

- Applied frame scaling to player/enemy horizontal movement, gravity, vertical integration, bullets, camera, background, animations, and comeback counters.
- Added fractional remainders for integer camera/background/bullet coordinates.
- Reset the frame clock after pumped modal waits so paused wall time is not simulated as catch-up movement.
- Replaced the truncated 16 ms cap with a fractional 16.667 ms target and retained delay remainder.

## Verification

- Warning-clean C++17 build with `-Wall -Wextra -Wpedantic`: passed.
- Focused split-step test: `0.5 + 0.5` produced the same camera displacement, bullet displacement, and enemy X/Y state as one `1.0` baseline step.
- Automated menu-to-gameplay smoke run produced a profiler interval and shut down with exit code 0.

The screen states remain modal and the simulation remains single-threaded. Consolidating those states into one dispatcher is a larger architecture change and was not required to make gameplay rates elapsed-time-aware in this step.
