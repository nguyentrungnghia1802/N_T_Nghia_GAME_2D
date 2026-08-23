#define SDL_MAIN_HANDLED
#include "../src/CommonFunc.h"

#include <cstdlib>
#include <iostream>

namespace
{
void Expect(bool condition, const char *name)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << name << '\n';
        std::exit(1);
    }
}
}

int main()
{
    using SDLCommonFunc::CheckCollision;

    const SDL_Rect base = {10, 10, 20, 20};
    Expect(!CheckCollision(base, {30, 10, 10, 10}), "right edge touch");
    Expect(!CheckCollision(base, {0, 10, 10, 10}), "left edge touch");
    Expect(!CheckCollision(base, {10, 30, 10, 10}), "bottom edge touch");
    Expect(!CheckCollision(base, {10, 0, 10, 10}), "top edge touch");
    Expect(CheckCollision(base, {15, 15, 2, 2}), "object 1 contains object 2");
    Expect(CheckCollision({15, 15, 2, 2}, base), "object 2 contains object 1");
    Expect(CheckCollision(base, {29, 15, 10, 10}), "right overlap");
    Expect(CheckCollision(base, {1, 15, 10, 10}), "left overlap");
    Expect(CheckCollision(base, {15, 29, 10, 10}), "bottom overlap");
    Expect(CheckCollision(base, {15, 1, 10, 10}), "top overlap");
    Expect(!CheckCollision(base, {15, 15, 0, 10}), "zero width");

    const SDL_Rect source = {7, 9, 999, 888};
    const SDL_Rect player_bullet = SDLCommonFunc::MakePlayerBulletHitbox(source);
    const SDL_Rect threat = SDLCommonFunc::MakeThreatHitbox(source);
    Expect(player_bullet.x == 7 && player_bullet.y == 9 &&
               player_bullet.w == 115 && player_bullet.h == 95,
           "player and bullet hitbox preserves legacy footprint");
    Expect(threat.x == 7 && threat.y == 9 &&
               threat.w == 150 && threat.h == 100,
           "threat hitbox preserves legacy footprint");

    std::cout << "collision tests passed\n";
    return 0;
}
