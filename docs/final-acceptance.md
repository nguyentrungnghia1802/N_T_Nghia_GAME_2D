# Final Refactor Acceptance

Automated gate completed on 2026-08-24 for `codex/refactor/full-optimization` after Steps 1-10.

## Gate results

| Gate | Result |
| --- | --- |
| Windows GNU Make rebuild from HEAD | Passed with `-Wall -Wextra -Wpedantic`, no diagnostics |
| Collision regression executable | Passed |
| Runtime-allocation/bullet-pool regression executable | Passed |
| Map ownership/restart/visible-range regression executable | Passed |
| Portable profiler regression executable | Passed |
| Application-source Win32 API audit | No direct Win32 header/API references outside vendored SDL headers |
| SDL startup and quit | Window created; process exited with code 0 |
| Git worktree after generated-artifact cleanup | Clean |

## Final settled-menu sample

The freshly built executable was allowed to settle for two seconds and then sampled for five seconds on the same 16-logical-processor Windows host used by the earlier audit.

| Metric | Result |
| --- | ---: |
| Whole-machine CPU | 0.019% |
| Single-core equivalent | 0.31% |
| Working set | 118.4 MiB |
| Shutdown | Exit code 0 |

This single sample confirms that the final menu is paced and light on this machine. It is not a low-end-hardware benchmark or a long-run leak test.

## Behavior and portability boundary

Focused tests preserve the original 60 Hz movement scale, camera/bullet split steps, legacy collision footprints, deterministic first shot, map restart state, resource ownership, and clean startup failure paths documented by the individual step reports. The earlier automated transition stress reached gameplay/death flow.

A complete manual level run, every journey screen, replay/win visual comparison, and long-duration leak tooling remain unconfirmed and are not represented as passed. Native Unix compilation was dry-run prepared but not executed on this host; Emscripten still requires the TD-010/R11 callback-style state-loop refactor.

## Git policy

Each refactor step is committed independently. `docs/agent.md` prohibits automatic push, so remote integration is intentionally left to an explicitly authorized release action.
