# Repository Agent Instructions

## Goal

Improve the SDL2 game without changing gameplay.

Priorities:

1.  Correctness.
2.  Resource safety.
3.  Runtime performance.
4.  Maintainability.
5.  Future portability.

## Before Every Task

Read:

1.  docs/task.md
2.  docs/refact.md
3.  Relevant project documentation.
4.  Related source files.

Only modify files required for the current task.

## Workflow

-   Work on one task only.
-   Do not combine tasks.
-   Do not jump ahead.
-   Report unrelated problems as TODO.

## Coding Rules

Prefer:

-   simple code.
-   low allocation.
-   clear ownership.
-   measurable improvements.

Avoid:

-   unnecessary rewrites.
-   frameworks.
-   speculative optimization.
-   gameplay changes.

## Testing

After every task:

-   build project.
-   verify startup.
-   verify affected behavior.

## Git

One completed task equals one commit.

Format:

refactor(step-X): description

Never push automatically.

## Report

After finishing:

-   Step completed.
-   Files modified.
-   Summary.
-   Remaining TODOs.
