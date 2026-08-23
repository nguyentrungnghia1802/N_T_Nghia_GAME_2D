# SDL2 Game Refactoring Roadmap

## Philosophy

Make the game faster, safer, and easier to maintain without changing
gameplay.

Order:

1.  Measure.
2.  Fix leaks and ownership.
3.  Remove expensive runtime operations.
4.  Improve update/render performance.
5.  Improve architecture gradually.

# Step 1

Create performance baseline.

Measure FPS, CPU, RAM, frame time, loading operations.

# Step 2

Fix resource ownership.

Handle:

-   textures.
-   sounds.
-   fonts.
-   enemies.
-   bullets.

Goal:

No leaks.

# Step 3

Cache all assets.

Remove texture loading from gameplay.

# Step 4

Improve game loop.

Replace:

-   frame-dependent movement.
-   blocking delays.
-   separated modal loops.

with controlled update/render flow.

# Step 5

Activate only nearby entities.

Inactive enemies should not consume CPU.

# Step 6

Optimize rendering.

Use visibility checks and avoid unnecessary drawing.

# Step 7

Optimize collision.

Use active lists and efficient queries.

# Step 8

Reduce runtime allocations.

Reuse objects and cached data.

# Step 9

Optimize map management.

Avoid unnecessary loading/copying.

# Step 10

Prepare portability.

Remove platform-specific runtime dependency.

## Success Criteria

The final game:

-   runs smoothly on low-end machines.
-   has stable frame time.
-   has no resource leaks.
-   keeps original gameplay.
