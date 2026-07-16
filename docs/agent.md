# Repository Agent Instructions

These rules apply to every coding task in this repository.

---

# Goal

The goal of this repository is to improve the existing SDL2 game without changing its gameplay.

Priorities:

1. Fix correctness issues.
2. Remove memory/resource leaks.
3. Improve runtime performance.
4. Improve maintainability.
5. Prepare the architecture for future WebAssembly (Emscripten) support.

Feature development is NOT the priority.

---

# Before Every Task

Before making any code changes:

1. Ensure the Git working tree is clean.

If there are uncommitted changes:

- Stop immediately.
- Ask the user whether to commit or stash them first.

Only continue when the repository is in a clean state.

Then read ONLY:

1. `docs/agent/AGENTS.md`
2. `docs/refact.md`
3. Source files directly related to the current task.

Do NOT scan the whole repository unless absolutely necessary.

Do NOT read:

- build/
- dist/
- binaries
- generated files
- third-party libraries
- SDL source
- large assets
- unrelated modules

---

# Workflow

The refactoring roadmap is defined in:

`docs/refact.md`

Always follow it sequentially.

Rules:

- Work on ONE step only.
- Never jump to another step.
- Never combine multiple steps into one task.
- If another issue is discovered, report it briefly as TODO instead of fixing it.

---

# Coding Rules

Only modify files required for the current task.

Avoid unnecessary changes.

Do NOT:

- refactor unrelated code
- change gameplay
- redesign architecture
- rename large parts of the project
- reformat the whole repository
- introduce new libraries
- introduce new frameworks
- perform speculative optimizations

Prefer:

- simple code
- readable code
- minimal changes
- low allocation
- low resource usage
- maintainable solutions

---

# Performance Principles

Prefer improvements that eliminate unnecessary work.

Examples:

- remove memory leaks
- remove resource leaks
- avoid repeated allocations
- avoid loading assets during gameplay
- avoid updating inactive entities
- avoid rendering invisible entities
- avoid unnecessary collision checks
- avoid unnecessary threads

Do not perform micro-optimizations without evidence.

---

# Testing

Do NOT create extensive tests.

Do NOT spend time writing benchmark code.

Only perform lightweight verification necessary to ensure the modified code still compiles and behaves correctly.

---

# Git

Each completed task corresponds to exactly ONE commit.

Commit message format:

refactor(step-X): short description

Examples:

refactor(step-1): add runtime profiler
refactor(step-2): fix resource ownership
refactor(step-3): cache runtime textures

Do NOT create a commit if the current step is incomplete.

Never push automatically.

---

# Report

After finishing the task, report only:

- Step completed
- Files modified
- Summary of changes
- Remaining TODOs (if any)

Do not create additional documentation unless explicitly requested.

Do not continue to the next step automatically.